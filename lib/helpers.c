#include "helpers.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

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

ssize_t read_until(int d, void *buf, size_t nbyte, char delimiter) {
  size_t total_bytes_read = 0;
  ssize_t bytes_read;
  while (total_bytes_read < nbyte) {
    bytes_read = read(d, (char *)buf + total_bytes_read, 1);
    if (bytes_read == -1) {
      return -1;
    } if (bytes_read == 0) {
      return total_bytes_read;
    } if (((char *)buf)[total_bytes_read++] == delimiter) {
      return total_bytes_read;
    }
  }
  return total_bytes_read;
}

int spawn(const char * file, char * const argv []) {
  pid_t process_id = fork();
  if (process_id == 0) {
    int devfd = open("/dev/null", O_WRONLY);
    dup2(devfd, STDOUT_FILENO);
    dup2(devfd, STDERR_FILENO);
    close(devfd);
    return execvp(file, argv);
  } else if (process_id > 0) {
    int exit_status;
    waitpid(process_id, &exit_status, 0);
    if (WIFEXITED(exit_status)) {
      return WEXITSTATUS(exit_status);
    } else {
      return -1;
    }
  }
  return -1;
}
