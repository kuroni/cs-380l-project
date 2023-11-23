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
#define QD  256
#define BS 16 * 1024

struct io_uring ring;

struct io_data {
    int read;
    int infd, outfd;
    size_t size;
    off_t offset;
    char* buf;
};

int setup_context(unsigned entries, struct io_uring *ring) {
    int ret = io_uring_queue_init(entries, ring, 0);
    if (ret < 0) {
        fprintf(stderr, "queue_init: %s\n", strerror(-ret));
        return -1;
    }
    return 0;
}

// Prep a queue job
void queue_prep(struct io_uring *ring, struct io_data *data) {
    struct io_uring_sqe *sqe;

    sqe = io_uring_get_sqe(ring);
    assert(sqe);
    assert(data);

    if (data->read) {
        io_uring_prep_read(sqe, data->infd, data->buf, data->size, data->offset);
    } else {
        io_uring_prep_write(sqe, data->outfd, data->buf, data->size, data->offset);
    }

    io_uring_sqe_set_data(sqe, data);
}

// Create a queue job that copies from infd to outfd @ size and offset
struct io_data *queue_create(int infd, int outfd, size_t size, off_t offset) {
    struct io_data *data = malloc(sizeof(struct io_data));
    if (data == NULL) {
        perror("malloc io_data");
        return NULL;
    }

    data->infd = infd;
    data->outfd = outfd;
    data->size = size;
    data->offset = offset;
    data->read = 1; // we only create in reads
    data->buf = malloc(sizeof(char) * (size + 1));
    if (data->buf == NULL) {
        perror("malloc buffer");
        return NULL;
    }

    return data;
}

int copy_file(struct io_uring *ring, int infd, int outfd, off_t insize) {
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

            struct io_data *data = queue_create(infd, outfd, this_size, offset);
            queue_prep(ring, data);

            insize -= this_size;
            offset += this_size;
            reads++;
        }

        if (had_reads != reads) {
            // submit multiple reads at the same time
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
                    queue_prep(ring, data);
                    io_uring_cqe_seen(ring, cqe);
                    continue;
                }
                fprintf(stderr, "cqe failed: %s\n",
                        strerror(-cqe->res));
                return 1;
            } else if (cqe->res != data->size) {
                /* short read/write; adjust and requeue */
                data->offset += cqe->res;
                data->size -= cqe->res;
                queue_prep(ring, data);
                io_uring_cqe_seen(ring, cqe);
                continue;
            }

            /*
             * All done. If write, nothing else to do. If read,
             * queue up corresponding write.
             * */

            if (data->read) {
                data->read = 0; // change to write
                queue_prep(ring, data);
                int ret = io_uring_submit(ring);
                if (ret < 0) {
                    perror("io_uring_submit write");
                }

                write_left -= data->size;
                reads--;
                writes++;
            } else {
                free(data->buf); // big bug here haha
                free(data);
                writes--;
            }
            io_uring_cqe_seen(ring, cqe);
        }
    }

    return 0;
}

int copy_recursive(char * const src[], char* dest) {
    FTS *ftsp;
    FTSENT *p;

    int fts_options = FTS_LOGICAL | FTS_NOCHDIR;
    if ((ftsp = fts_open(src, fts_options, NULL)) == NULL) {
        warn("fts_open");
        return 1;
    }

    int src_len = strlen(src[0]), dest_len = strlen(dest);
    char* dest_file = (char*)malloc(BUF);
    strcpy(dest_file, dest);

    while ((p = fts_read(ftsp)) != NULL) {
        switch (p->fts_info) {
        case FTS_D:
            // make a new directory
            strcpy(dest_file + dest_len, p->fts_path + src_len);
            // fprintf(stderr, "make d %s\n", dest_file);
            mkdir(dest_file, 0700); // we don't care if this fails, we only need the guarantee that there is a directory after
            break;
        case FTS_F:
            strcpy(dest_file + dest_len, p->fts_path + src_len);
            // fprintf(stderr, "copy f %s -> %s\n", p->fts_path, dest_file);


            int infd = open(p->fts_path, O_RDONLY);
            if (infd < 0) {
                perror("open infile");
                return 1;
            }
            int outfd = open(dest_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (outfd < 0) {
                perror("open outfile");
                return 1;
            }
            int ret = copy_file(&ring, infd, outfd, p->fts_statp->st_size);
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

    int rc = copy_recursive(argv + 1, dest);
    io_uring_queue_exit(&ring);

    return rc;
}
