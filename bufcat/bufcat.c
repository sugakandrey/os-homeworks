#include "bufio.h"
#include <stdio.h>
#include <unistd.h>


const size_t BUFFER_SIZE = 4096;

void on_err(struct buf_t * buf) {
  if (buf_size(buf) > 0) {
    buf_flush(STDOUT_FILENO, buf, BUFFER_SIZE);
  }
  buf_free(buf);
}

int main(void) {
  struct buf_t * buf = buf_new(BUFFER_SIZE);
  if (buf == NULL) {
    return -1;
  }
  ssize_t read, written;
  while ((read = buf_fill(STDIN_FILENO, buf, BUFFER_SIZE)) > 0) {
    written = buf_flush(STDOUT_FILENO, buf, BUFFER_SIZE);
    if (read < 0 || written < 0) {
      on_err(buf);
      return -1;
    }
  }
  buf_free(buf);
  return 0;
}
