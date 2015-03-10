#include "helpers.h"
#include "errno.h"

ssize_t read_(int d, void *buf, size_t nbyte) {
  size_t total_bytes_read = 0;
  ssize_t bytes_read;
  while (total_bytes_read < nbyte) {
    bytes_read = read(d, (char *)buf + total_bytes_read, nbyte - total_bytes_read);
    if (bytes_read <= 0) {
      break;
    } else {
      total_bytes_read += bytes_read;
    }
  }
  if (bytes_read == -1) {
    return -1;
  }
  return total_bytes_read;
}

ssize_t write_(int d, const void *buf, size_t nbyte) {
  size_t total_bytes_written = 0;
  ssize_t bytes_written;
  while (total_bytes_written < nbyte){
    bytes_written = write(d, (char *)buf + total_bytes_written, nbyte - total_bytes_written);
    if (bytes_written <= 0) {
      break;
    } else {
      total_bytes_written += bytes_written;
    }
  }
  if (bytes_written == -1) {
    return -1;
  }
  return total_bytes_written;
}
