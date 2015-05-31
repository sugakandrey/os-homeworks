#define _GNU_SOURCE
#include "helpers.h"
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

const size_t BUFFER_SIZE = 8192;

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
    /* int devfd = open("/dev/null", O_WRONLY); */
    /* dup2(devfd, STDOUT_FILENO); */
    /* dup2(devfd, STDERR_FILENO); */
    /* close(devfd); */
    exit(execvp(file, argv));
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

execargs_t* execargs_new(char* file, char** args, size_t args_n) {
  execargs_t* program = malloc(sizeof(execargs_t));
  if (program) {
    program->args_n = args_n;
    program->file = file;
    program->args = args;
  }
  return program;
}

void execargs_free(execargs_t* args) {
  for (char** it = args->args; *it; it++) {
    free(*it);
  }
  free(args);
}


static int process_count;
static int* process;
static struct sigaction old;

void kill_procs(void) {
  for (int i = 0; i < process_count; i++) {
    kill(process[i], SIGKILL);
  }
  process_count = 0;
}

void report_error_and_exit_helper(const char* message, void (*exit_function)(int)) {
  fprintf(stderr, "Error: %s, errno: %s\n", message, strerror(errno));
  kill_procs();
  exit_function(EXIT_FAILURE);
}

void report_error_and_exit(const char* message) {
  report_error_and_exit_helper(message, exit);
}

void _report_error_and_exit(const char* message) {
  report_error_and_exit_helper(message, _exit);
}

void redirect(int fd_old, int fd_new) {
  if (fd_old != fd_new) {
    if (dup2(fd_old, fd_new) != -1) {
      close(fd_old);
    } else {
      _report_error_and_exit("dup2 failed");
    }
  }
}

int exec(execargs_t* args) {
  return spawn(args->file, args->args);
}

int exec_redirected(execargs_t* program, int in_fd, int out_fd) {
  redirect(in_fd, STDIN_FILENO);
  redirect(out_fd, STDOUT_FILENO);
  return exec(program);
}

void handler(int signal) {
  kill_procs();
  sigaction(SIGINT, &old, NULL);
}

int runpiped(execargs_t** programs, size_t n) {
  int pipesfd[n - 1][2];
  int procs[n];
  memset(procs, -1, n);
  process_count = n;

  for (size_t i = 0; i < n - 1; i++) {
    if (pipe2(pipesfd[i], O_CLOEXEC) == -1) {
      report_error_and_exit("pipe2 failed");
    }
  }
  for (size_t i = 0; i < n; i++) {
    procs[i] = fork();
    if (procs[i] == 0) {
      if (i != 0) {
        dup2(pipesfd[i - 1][0], STDIN_FILENO);
      }
      if (i != n - 1) {
        dup2(pipesfd[i][1], STDOUT_FILENO);
      }
      execvp(programs[i]->file, programs[i]->args);
      _report_error_and_exit("execvp failed");
    }
    if (procs[i] == -1) {
      report_error_and_exit("fork failed");
    }
  }
  for (size_t i = 0; i < n; i++) {
    close(pipesfd[i][1]);
    close(pipesfd[i][0]);
  }
  process = procs;

  struct sigaction sig_act;
  sig_act.sa_handler = handler;
  sig_act.sa_flags = 0;
  sigemptyset(&sig_act.sa_mask);
  if (sigaction(SIGINT, &sig_act, &old) == -1) {
    report_error_and_exit("sigaction failed");
  }
  for (size_t i = 0; i < n; i++) {
    int exit_status;
    waitpid(process[i], &exit_status, 0);
  }
  process_count = 0;
  sigaction(SIGINT, &old, NULL);
  return 0;
}
