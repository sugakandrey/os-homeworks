#include "../lib/bufio.h"
#include "../lib/helpers.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>

typedef struct addrinfo addrinfo;

const size_t BUFFER_SIZE = 8192;
const size_t CONNECTIONS_LIMIT = 128;

static void print_usage() {
  write_(STDOUT_FILENO, "Usage: ./bipiper %from_port% %to_port%\n", 39);
}

void get_info(const char* serv_name, addrinfo** res, addrinfo* hints) {
  if (getaddrinfo("localhost", serv_name, hints, res)) {
    report_error_and_exit("getaddrinfo failed");
  }
}

void open_socket(int* sock, addrinfo* localhost_info) {
  for (addrinfo* info = localhost_info; info; info = info->ai_next) {
    *sock = socket(info->ai_family, info->ai_socktype | SOCK_NONBLOCK, info->ai_protocol);
    if (*sock == -1) continue;
    int bind_res = bind(*sock, info->ai_addr, info->ai_addrlen);
    if (!bind_res) break;
    close(*sock);
  }
  if (*sock == -1) {
    report_error_and_exit("error creating/binding socket");
  }
  if (listen(*sock, CONNECTIONS_LIMIT) == -1) {
    report_error_and_exit("listen failed");
  }
  freeaddrinfo(localhost_info);
}

int init_server(const char* port) {
  int sock = -1;
  addrinfo* server_info;
  addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = PF_UNSPEC;
  get_info(port, &server_info, &hints);
  open_socket(&sock, server_info);
  return sock;
}

void _connect(int fd_from, int fd_to) {
  printf("connected from %d with to %d\n", fd_from, fd_to);
  buf_t* buf = buf_new(BUFFER_SIZE);
  if (!buf) {
    report_error_and_exit("failed to create buffer");
  }
  while(1) {
    ssize_t flushed;
    ssize_t filled = buf_fill(fd_from, buf, 1);
    if (!filled) break;
    if (filled == 1) {
      flushed = buf_flush(fd_to, buf, 1);
      if (flushed == -1) {
        report_error_and_exit("failed to write from buf");
      }
      report_error_and_exit("failed to fill buf");
    }
    flushed = buf_flush(fd_to, buf, 1);
    if (flushed == -1) {
      report_error_and_exit("failed to write from buf");
    }
  }
  buf_free(buf);
}

int main(int argc, char** argv) {
  if (argc < 3) {
    print_usage();
    return EXIT_FAILURE;
  }
  int server_fds[2];
  server_fds[0] = init_server(argv[1]);
  server_fds[1] = init_server(argv[2]);
  while(1) {
    int client_fd1 = accept4(server_fds[0], NULL, NULL, SOCK_NONBLOCK);
    if (client_fd1 == -1) {
      report_error_and_exit("accept failed on first socket");
    }
    int client_fd2 = accept4(server_fds[1], NULL, NULL, SOCK_NONBLOCK);
    if (client_fd2 == -1) {
      report_error_and_exit("accept failed on second socket");
    }
    pid_t pid = fork();
    printf("forked first time");
    if (pid == -1) {
      report_error_and_exit("first fork failed");
    } else if (!pid) {
      close(server_fds[0]);
      close(server_fds[1]);
      _connect(client_fd1, client_fd2);
      _exit(EXIT_SUCCESS);
    }

    pid_t pid2 = fork();
    printf("forked second time");
    if (pid2 == -1) {
      kill(pid, SIGKILL);
      report_error_and_exit("second fork failed");
    } else if (!pid) {
      close(server_fds[0]);
      close(server_fds[1]);
      _connect(client_fd2, client_fd1);
      _exit(EXIT_SUCCESS);
    } else {
      close(client_fd1);
      close(client_fd2);
    }
  }
  return 0;
}
