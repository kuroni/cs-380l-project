#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <liburing.h>
#include <fts.h>
#include <err.h>

#define BUF 256

#define QD  1024
#define BS (16 * 1024)

int infd, outfd;
struct io_uring ring;

struct io_data {
    int read;
    off_t first_offset, offset;
    size_t first_len;
    struct iovec iov;
};

int setup_context(unsigned entries, struct io_uring *ring) {
    int ret;

    ret = io_uring_queue_init(entries, ring, 0);
    if( ret < 0) {
        fprintf(stderr, "queue_init: %s\n", strerror(-ret));
        return -1;
    }

    return 0;
}

void queue_prepped(struct io_uring *ring, struct io_data *data) {
    struct io_uring_sqe *sqe;

    sqe = io_uring_get_sqe(ring);
    assert(sqe);

    if (data->read)
        io_uring_prep_readv(sqe, infd, &data->iov, 1, data->offset);
    else
        io_uring_prep_writev(sqe, outfd, &data->iov, 1, data->offset);

    io_uring_sqe_set_data(sqe, data);
}

int queue_read(struct io_uring *ring, off_t size, off_t offset) {
    fprintf(stderr, "queue read %d %ld %ld\n", infd, size, offset);
    struct io_uring_sqe *sqe;
    struct io_data *data;

    data = malloc(size + sizeof(*data));
    if (!data)
        return 1;

    sqe = io_uring_get_sqe(ring);
    if (!sqe) {
        free(data);
        return 1;
    }

    data->read = 1;
    data->offset = data->first_offset = offset;

    data->iov.iov_base = data + 1;
    data->iov.iov_len = size;
    data->first_len = size;

    io_uring_prep_readv(sqe, infd, &data->iov, 1, offset);
    io_uring_sqe_set_data(sqe, data);
    return 0;
}

void queue_write(struct io_uring *ring, struct io_data *data) {
    fprintf(stderr, "queue write %d %ld %ld\n", outfd, data->first_len, data->offset);
    data->read = 0;
    data->offset = data->first_offset;

    data->iov.iov_base = data + 1;
    data->iov.iov_len = data->first_len;

    queue_prepped(ring, data);
    io_uring_submit(ring);
}

int copy_file(struct io_uring *ring, off_t insize) {
    unsigned long reads, writes;
    struct io_uring_cqe *cqe;
    off_t write_left, offset;
    int ret;

    write_left = insize;
    writes = reads = offset = 0;

    while (insize || write_left) {
        int had_reads, got_comp;

        /* Queue up as many reads as we can */
        had_reads = reads;
        while (insize) {
            off_t this_size = insize;

            if (reads + writes >= QD)
                break;
            if (this_size > BS)
                this_size = BS;
            else if (!this_size)
                break;

            if (queue_read(ring, this_size, offset))
                break;

            insize -= this_size;
            offset += this_size;
            reads++;
        }

        if (had_reads != reads) {
            ret = io_uring_submit(ring);
            if (ret < 0) {
                fprintf(stderr, "io_uring_submit: %s\n", strerror(-ret));
                break;
            }
        }

        /* Queue is full at this point. Let's find at least one completion */
        got_comp = 0;
        while (write_left) {
            struct io_data *data;

            if (!got_comp) {
                ret = io_uring_wait_cqe(ring, &cqe);
                got_comp = 1;
            } else {
                ret = io_uring_peek_cqe(ring, &cqe);
                if (ret == -EAGAIN) {
                    cqe = NULL;
                    ret = 0;
                }
            }
            if (ret < 0) {
                fprintf(stderr, "io_uring_peek_cqe: %s\n",
                        strerror(-ret));
                return 1;
            }
            if (!cqe)
                break;

            data = io_uring_cqe_get_data(cqe);
            if (cqe->res < 0) {
                if (cqe->res == -EAGAIN) {
                    queue_prepped(ring, data);
                    io_uring_cqe_seen(ring, cqe);
                    continue;
                }
                fprintf(stderr, "cqe failed: %s\n",
                        strerror(-cqe->res));
                return 1;
            } else if (cqe->res != data->iov.iov_len) {
                /* short read/write; adjust and requeue */
                data->iov.iov_base += cqe->res;
                data->iov.iov_len -= cqe->res;
                queue_prepped(ring, data);
                io_uring_cqe_seen(ring, cqe);
                continue;
            }

            /*
             * All done. If write, nothing else to do. If read,
             * queue up corresponding write.
             * */

            if (data->read) {
                queue_write(ring, data);
                write_left -= data->first_len;
                reads--;
                writes++;
            } else {
                free(data);
                writes--;
            }
            io_uring_cqe_seen(ring, cqe);
        }
    }

    return 0;
}

int copy_r(char * const src[], char* dest) {
    FTS *ftsp;
    FTSENT *p;

    int fts_options = FTS_COMFOLLOW | FTS_LOGICAL | FTS_NOCHDIR;
    if ((ftsp = fts_open(src, fts_options, NULL)) == NULL) {
        warn("fts_open");
        return 1;
    }

    int srclen = strlen(src[0]), destlen = strlen(dest);
    char* destfile = (char*)malloc(BUF);
    strcpy(destfile, dest);

    while ((p = fts_read(ftsp)) != NULL) {
        switch (p->fts_info) {
        case FTS_D:
            // make a new directory
            strcpy(destfile + destlen, p->fts_path + srclen);
            fprintf(stderr, "make d %s\n", destfile);
            mkdir(destfile, 0700); // we don't care if this fails, we only need the guarantee that there is a directory after
            break;
        case FTS_F:
            strcpy(destfile + destlen, p->fts_path + srclen);
            fprintf(stderr, "copy f %s\n", destfile);
            infd = open(p->fts_path, O_RDONLY);
            if (infd < 0) {
                perror("open infile");
                return 1;
            }

            outfd = open(destfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (outfd < 0) {
                perror("open outfile");
                return 1;
            }

            int ret = copy_file(&ring, p->fts_statp->st_size);
            close(infd);
            close(outfd);
            if (ret != 0) {
                return ret;
            }
            break;
        default:
            break;
        }
    }
    fts_close(ftsp);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <src> <dest>\n", argv[0]);
        return 1;
    }

    if (setup_context(QD, &ring)) {
        return 1;
    }

    // create new destination position
    char* dest = argv[argc - 1];
    argv[argc - 1] = NULL;

    int rc = copy_r(argv + 1, dest);
    io_uring_queue_exit(&ring);

    return rc;
}
