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

struct io_uring ring;

// represents a copy request from infd to outfd
// maintains the number of reads on infd and number of writes on outfd
// must use the same copy_data for different queue jobs on the same file
struct copy_data {
    int infd, outfd;
    int reads, writes;
    size_t insize;
    off_t offset;
};

struct io_data {
    int isread;
    struct copy_data *cd;
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

    if (data->isread)
        io_uring_prep_read(sqe, data->cd->infd, data->buf, data->size, data->offset);
    else
        io_uring_prep_write(sqe, data->cd->outfd, data->buf, data->size, data->offset);

    io_uring_sqe_set_data(sqe, data);
}

// Create a queue job that copies from infd to outfd @ size and offset
struct io_data *queue_create(struct copy_data *cd, size_t size, off_t offset) {
    if (cd == NULL) {
        fprintf(stderr, "current copy_file struct is null\n");
        return NULL;
    }

    struct io_data *data = malloc(sizeof(struct io_data));
    if (data == NULL) {
        perror("malloc io_data");
        return NULL;
    }

    data->cd = cd;
    data->size = size;
    data->offset = offset;
    data->isread = 1; // we only create in reads
    data->buf = malloc(sizeof(char) * (size + 1));
    if (data->buf == NULL) {
        perror("malloc buffer");
        return NULL;
    }

    return data;
}

struct copy_data *create_copy_data(char *src, char *dest, size_t size) {
    struct copy_data *cp = malloc(sizeof(struct copy_data));
    if (cp == NULL) {
        perror("malloc copy_data");
        return NULL;
    }

    cp->infd = open(src, O_RDONLY);
    if (cp->infd < 0) {
        perror("open infile");
        return NULL;
    }
    cp->outfd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (cp->outfd < 0) {
        perror("open outfile");
        return NULL;
    }
    cp->insize = size;
    cp->offset = 0;
    cp->reads = 0;
    cp->writes = 0;
    
    return cp;
}

int copy_recursive(struct io_uring *ring, char * const src[], char* dest) {
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

    struct io_uring_cqe *cqe;
    struct copy_data *curfile = NULL;
    int curdepth = 0, newreads, gotcomp, ret;

    do {
        // Queue up all the reads
        newreads = 0;
        while (curdepth < QD) {
            if (curfile == NULL) {
                // get the next file to be read
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
                        fprintf(stderr, "copy f %s -> %s\n", p->fts_path, destfile);
                        curfile = create_copy_data(p->fts_path, destfile, p->fts_statp->st_size);
                        break;
                    default:
                        break;
                    }
                }
            }

            if (curfile == NULL) {
                // no file left to copy
                break;
            }
            while (curfile->insize && curdepth < QD) {
                size_t cursize = curfile->insize;
                if (cursize > BS) {
                    cursize = BS;
                }

                struct io_data *data = queue_create(curfile, cursize, curfile->offset);
                queue_prep(ring, data);

                curfile->insize -= cursize;
                curfile->offset += cursize;
                curfile->reads++;
                newreads = 1;

                curdepth++;
            }

            if (curfile->insize == 0) {
                // this might seem like a dangling pointer but it's not
                // we leave the curfile struct allocated so queued jobs
                // can access them
                curfile = NULL;
            }
        }

        if (newreads) {
            // submit multiple reads at the same time
            ret = io_uring_submit(ring);
            if (ret < 0) {
                fprintf(stderr, "io_uring_submit: %s\n", strerror(-ret));
                break;
            }
        }

        gotcomp = 0;
        while (curdepth) {
            struct io_data *data;

            if (!gotcomp) {
                ret = io_uring_wait_cqe(ring, &cqe);
                gotcomp = 1;
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

            if (data->isread) {
                data->isread = 0; // change to write
                data->cd->reads--;
                data->cd->writes++;
                if (!data->cd->reads && !data->cd->insize) {
                    // we don't need any more read on this file
                    // so we close the read fd
                    close(data->cd->infd);
                }

                queue_prep(ring, data);
                int ret = io_uring_submit(ring);
                if (ret < 0) {
                    perror("io_uring_submit write");
                }
            } else {
                data->cd->writes--;
                if (!data->cd->writes && !data->cd->insize) {
                    // we don't need any more write on this queue
                    // so we close the write fd AND free the copy_data struct
                    close(data->cd->outfd);
                    free(data->cd);
                }
                free(data);
                curdepth--;
            }
            io_uring_cqe_seen(ring, cqe);
        }
    } while (curdepth || curfile);

    
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

    int rc = copy_recursive(&ring, argv + 1, dest);
    io_uring_queue_exit(&ring);

    return rc;
}
