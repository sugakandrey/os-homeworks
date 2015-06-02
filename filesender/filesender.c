#include "../lib/bufio.h"
#include "../lib/helpers.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>

typedef struct addrinfo addrinfo;

const size_t BUFFER_SIZE = 8192;
const size_t CONNECTIONS_LIMIT = 128;

static void print_usage() {
  write_(STDOUT_FILENO, "Usage: ./filesender %port% %filename%\n", 38);
}

void get_info(const char* serv_name, addrinfo** res, addrinfo* hints) {
  if (getaddrinfo("localhost", serv_name, hints, res)) {
    report_error_and_exit("getaddrinfo failed");
  }
}

void open_socket(int* sock, addrinfo* localhost_info) {
  for (addrinfo* info = localhost_info; info; info = info->ai_next) {
    *sock = socket(info->ai_family, SOCK_STREAM, info->ai_protocol);
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

void send_file(int fd, char* file) {
  buf_t* buf = buf_new(BUFFER_SIZE);
  if (!buf) {
    report_error_and_exit("failed to create buf");
  }
  int file_fd = open(file, O_RDONLY);
  if (file_fd == -1) {
    report_error_and_exit("failed to open file");
  }
  while (1) {
    ssize_t filled = buf_fill(file_fd, buf, 1);
    if (!filled) break;
    if (filled == -1) {
      ssize_t flushed = buf_flush(fd, buf, 1);
      if (flushed == -1) {
        report_error_and_exit("failed to flush buf");
      }
      report_error_and_exit("failed to fill buf");
    }
    ssize_t flushed = buf_flush(fd, buf, 1);
    if (flushed == -1) {
      report_error_and_exit("failed to flush buf");
    }
  }
  buf_free(buf);
}

int main(int argc, char** argv) {
  if (argc < 3) {
    print_usage();
    return EXIT_FAILURE;
  }
  int sock = -1;
  addrinfo* localhost_info;
  addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_UNSPEC;

  get_info(argv[1], &localhost_info, &hints);
  open_socket(&sock, localhost_info);
  while(1) {
    int client_fd = accept(sock, NULL, NULL);
    if (client_fd == -1) {
      printf("wtf!");
      report_error_and_exit("accept failed");
    }
    pid_t pid = fork();
    if (pid == -1) {
      report_error_and_exit("fork failed");
    } else if (!pid) {
      close(sock);
      send_file(client_fd, argv[2]);
      _exit(EXIT_SUCCESS);
    } else {
      close(client_fd);
    }
  }
  return 0;
}
