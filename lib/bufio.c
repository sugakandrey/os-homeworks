#include "bufio.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct buf_t Buffer;

#ifdef DEBUG
#define ASSERT(cond) if (!(cond)) abort();
#else
#define ASSERT(buf)
#endif

Buffer* buf_new(size_t capacity) {
  Buffer* buf = malloc(sizeof(Buffer));
  if (buf == NULL) {
    return NULL;
  }
  buf->data = malloc(capacity);
  if (buf->data == NULL) {
    free(buf);
    return NULL;
  }
  buf->size = 0;
  buf->capacity = capacity;
  return buf;
}

void buf_free(Buffer* buf) {
  ASSERT(buf != NULL);
  free(buf->data);
  free(buf);
}

size_t buf_capacity(Buffer* buf) {
  ASSERT(buf != NULL)
    return buf->capacity;
}

size_t buf_size(Buffer* buf) {
  ASSERT(buf != null);
  return buf->size;
}

ssize_t buf_fill(fd_t fd, Buffer* buf, size_t required) {
  ASSERT(buf != NULL && required <= buf->size);
  ssize_t bytes_read;
  while (buf->size < required) {
    bytes_read = read(fd, buf->data + buf->size, buf->capacity - buf->size);
    if (bytes_read == -1) {
      return -1;
    } else if (bytes_read == 0) {
      break;
    }
    buf->size += bytes_read;
  }
  return buf->size;
}

ssize_t buf_flush(fd_t fd, Buffer* buf, size_t required) {
  ASSERT(buf != NULL);
  size_t total_bytes_written = 0;
  while (buf->size > total_bytes_written && total_bytes_written < required) {
    ssize_t bytes_written = write(fd, buf->data + total_bytes_written, buf->size - total_bytes_written);
    if (bytes_written == -1) {
      return -1;
    }
    total_bytes_written += bytes_written;
  }
  buf->size -= total_bytes_written;
  if (buf->size > 0) {
    memcpy(buf->data, buf->data + total_bytes_written, buf->size - total_bytes_written);
  }
  return total_bytes_written;
}
