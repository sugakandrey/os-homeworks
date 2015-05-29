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

void report_error_and_exit_helper(const char* message, void (*exit_function)(int)) {
  fprintf(stderr, "Error: %s, errno: %s\n", message, strerror(errno));
  exit_function(EXIT_FAILURE);
}

void report_error_and_exit(const char* message) {
  report_error_and_exit_helper(message, exit);
}

void _report_error_and_exit(const char* message) {
  report_error_and_exit_helper(message, _exit);
}

static int stdin_orig;
static int stdout_orig;
static int current_process;
static struct sigaction old;

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
  if (signal == SIGINT) {
    if (current_process) {
      kill(current_process, SIGKILL);
      waitpid(current_process, NULL, 0);
    }
  }
}

int runpiped(execargs_t** programs, size_t n) {
  stdin_orig = dup(STDIN_FILENO);
  stdout_orig = dup(STDOUT_FILENO);

  struct sigaction sig_act;
  sig_act.sa_handler = handler;
  sig_act.sa_flags = 0;
  sigemptyset(&sig_act.sa_mask);
  sigaction(SIGINT, &sig_act, &old);

  size_t i = 0;
  int in = STDIN_FILENO;
  for (; i < n - 1; i++) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
      report_error_and_exit("pipe failed");
    }
    pid_t pid;
    pid = fork();
    if (pid == -1) {
      report_error_and_exit("fork failed");
    } else if (pid == 0) {
      close(pipefd[0]);
      exit(exec_redirected(programs[i], in, pipefd[1]));
    } else {
      int status;
      current_process = pid;
      close(pipefd[1]);
      close(in);
      in = pipefd[0];
      waitpid(pid, &status, 0);
      if (!WIFEXITED(status)) {
        return EXIT_FAILURE;
      }
    }
  }
  int res = exec_redirected(programs[i], in, STDOUT_FILENO);
  current_process = 0;
  dup2(stdin_orig, STDIN_FILENO);
  return res;
}
