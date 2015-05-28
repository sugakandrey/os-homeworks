#ifndef _BUFIO_
#define _BUFIO_
#include <stddef.h>
#include <sys/types.h>

struct buf_t {
    size_t capacity;
    size_t size;
  void* data;
};

typedef int fd_t;
extern struct buf_t *buf_new(size_t capacity);
extern void buf_free(struct buf_t *);
extern size_t buf_capacity(struct buf_t *);
extern size_t buf_size(struct buf_t *);
extern ssize_t buf_fill(fd_t fd, struct buf_t *buf, size_t required);
extern ssize_t buf_flush(fd_t fd, struct buf_t *buf, size_t required);

#endif

