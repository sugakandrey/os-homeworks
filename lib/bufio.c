#include "bufio.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#ifdef DEBUG
#define ASSERT(cond) if (!(cond)) abort();
#else
#define ASSERT(buf)
#endif

buf_t* buf_new(size_t capacity) {
  buf_t* buf = malloc(sizeof(buf_t));
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

void buf_free(buf_t* buf) {
  ASSERT(buf != NULL);
  free(buf->data);
  free(buf);
}

size_t buf_capacity(buf_t* buf) {
  ASSERT(buf != NULL)
    return buf->capacity;
}

size_t buf_size(buf_t* buf) {
  ASSERT(buf != null);
  return buf->size;
}

ssize_t buf_fill(fd_t fd, buf_t* buf, size_t required) {
  ASSERT(buf != NULL);
  ASSERT(required <= buf->size);
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

ssize_t buf_flush(fd_t fd, buf_t* buf, size_t required) {
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

size_t find_newline(buf_t* buf, char* dest) {
  ASSERT(buf != null);
  for (size_t i = 0; i < buf->size; i++) {
    if (buf->data[i] == '\n') {
      buf->data[i] = '\0';
      memcpy(dest, buf->data, i + 1);
      memcpy(buf->data, buf->data + i + 1, buf->size - i - 1);
      return i;
    }
  }
  return 0;
}

ssize_t buf_getline(fd_t fd, buf_t* buf, char* dest) {
  ASSERT(buf != NULL);
  size_t found = find_newline(buf, dest);
  if (found) {
    buf->size -= found + 1;
    return found;
  }
  while (buf->size <= buf->capacity) {
    size_t init_size = buf->size;
    ssize_t filled = buf_fill(fd, buf, init_size + 1);
    if (filled == -1) {
      return -1;
    } else if (filled == init_size) {
      return 0;
    }
    size_t len = find_newline(buf, dest);
    if (len) {
      buf->size -= len + 1;
      return len;
    }
  }
  return 0;
}
