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
#include <poll.h>
#include <errno.h>

#ifndef POLLRDHUP
#define POLLRDHUP 0x2000
#endif

typedef struct addrinfo addrinfo;

#define TRY(X, MSG) \
  do { \
    if ((X) == -1) report_error_and_exit(MSG); \
  } while (0)

const size_t BUFFER_SIZE = 8192;
const size_t CONNECTIONS_LIMIT = 128;
#define MAX_CONNECTIONS 128
#define TERMINATED 1

static void print_usage() {
  write_(STDOUT_FILENO, "Usage: ./polling %from_port% %to_port%\n", 39);
}

void get_info(const char* serv_name, addrinfo** res, addrinfo* hints) {
  TRY(getaddrinfo("10.0.2.15", serv_name, hints, res), "getaddrinfo failed");
}

void open_socket(int* sock, addrinfo* localhost_info) {
  for (addrinfo* info = localhost_info; info; info = info->ai_next) {
    *sock = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
    if (*sock == -1) continue;
    int bind_res = bind(*sock, info->ai_addr, info->ai_addrlen);
    if (!bind_res) break;
    close(*sock);
  }
  TRY(*sock, "error creating/binding socket");
  TRY(listen(*sock, CONNECTIONS_LIMIT), "listen failed");
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

struct pollfd pollfds[MAX_CONNECTIONS * 2];
buf_t* buffers[MAX_CONNECTIONS * 2];
size_t server_state = 0;
size_t connection_state[MAX_CONNECTIONS * 2];
size_t current_size = 2;

void _accept(int socket, int* fd) {
  *fd = accept(pollfds[socket].fd, NULL, NULL);
  printf("accepting client %zu with fd %d\n", current_size, *fd);
  pollfds[socket].events = 0;
  pollfds[socket ^ 1].events = POLLIN;
  server_state ^= 1;
}

#define swap(TYPE, A, B) { TYPE tmp = A; A = B; B = tmp; }

void flush(int i) {
  if (buffers[i]->size > 0) {
    while (1) {
      ssize_t flushed = buf_flush(pollfds[i ^ 1].fd, buffers[i], 1);
      if (!flushed || flushed == -1) break;
    }
  }
}

void close_pair(int i) {
  printf("closing pair %d\n", i);
  pollfds[0].events |= POLLIN;
  flush(i);
  flush(i ^ 1);
  memset(buffers[i]->data, 0, BUFFER_SIZE);
  memset(buffers[i ^ 1]->data, 0, BUFFER_SIZE);
  close(pollfds[i].fd);
  close(pollfds[i ^ 1].fd);
  if (buffers[i]) {
    buf_free(buffers[i]);
    buffers[i] = NULL;
  }
  if (buffers[i ^ 1]) {
    buf_free(buffers[i ^ 1]);
    buffers[i ^ 1] = NULL;
  }
  connection_state[i] = 0;
  connection_state[i ^ 1] = 0;
  i &= ~1;
  swap(buf_t*, buffers[i], buffers[current_size - 2]);
  swap(buf_t*, buffers[i + 1], buffers[current_size - 1]);
  swap(struct pollfd, pollfds[i], pollfds[current_size - 2]);
  swap(struct pollfd, pollfds[i + 1], pollfds[current_size - 1]);
  swap(int, connection_state[i], connection_state[current_size - 2]);
  swap(int, connection_state[i + 1], connection_state[current_size - 1]);
  current_size -= 2;
}

void init_client(int fd) {
  pollfds[current_size].fd = fd;
  pollfds[current_size].events = POLLIN;
  buffers[current_size] = buf_new(BUFFER_SIZE);
}

int main(int argc, char** argv) {
  if (argc < 3) {
    print_usage();
    return EXIT_FAILURE;
  }
  memset(pollfds, 0, sizeof(struct pollfd) * MAX_CONNECTIONS);
  memset(buffers, 0, sizeof(buffers));
  memset(connection_state, 0, sizeof(connection_state));
  pollfds[0].fd = init_server(argv[1]);
  pollfds[0].events = POLLIN;
  pollfds[1].fd = init_server(argv[2]);
  int client_fd1 = -1;
  int client_fd2 = -1;
  ssize_t total = 0;
  while (1) {
    int poll_res = poll(pollfds, current_size, -1);
    if (poll_res == -1) {
      if (errno == EINTR) continue;
      report_error_and_exit("poll failed");
    } else if (!poll_res) continue;
    for (size_t i = 0; i < current_size; i++) {
      short revents = pollfds[i].revents;
      if (revents & POLLIN) {
        if (i == 0) {
          _accept(0, &client_fd1);
        } else if (i == 1) {
            _accept(1, &client_fd2);
        } else {
          buf_t* buf = buffers[i];
          ssize_t old_size = buf->size;
          ssize_t filled = buf_fill(pollfds[i].fd, buf, 1);
          printf("POLLIN on client %zu bytes_read = %ld\n", i, filled);
          total += filled;
          if (filled == -1) {
            close_pair(i);
            continue;
          }
          if (filled == old_size) {
            printf("filled == oldsize => terminated read end\n");
            printf("total bytes send = %ld\n", total);
            pollfds[i].events &= ~POLLIN;
            shutdown(pollfds[i].fd, SHUT_RD);
            connection_state[i] = TERMINATED;
            if (connection_state[i ^ 1] & TERMINATED) close_pair(i);
          }
          if (buf->size == buf->capacity) {
            pollfds[i].events &= ~POLLIN;
          }
          if (buf->size > 0) {
            pollfds[i ^ 1].events |= POLLOUT;
          }
        }
      }
      if (revents & POLLOUT) {
        buf_t* buf = buffers[i ^ 1];
        ssize_t flushed = buf_flush(pollfds[i].fd, buf, 1);
        printf("POLLOUT on client %zu, bytes written = %ld\n", i, flushed);
        if (flushed == -1) {
          close_pair(i);
          continue;
        }
        if (!buf->size) {
          pollfds[i].events &= ~POLLOUT;
        }
        if (buf->size < buf->capacity) {
          pollfds[i ^ 1].events |= POLLIN;
        }
      }
      if (client_fd1 != -1 && client_fd2 != -1) {
        init_client(client_fd1);
        current_size++;
        init_client(client_fd2);
        current_size++;
        if (current_size == MAX_CONNECTIONS) {
          pollfds[0].events = 0;
          pollfds[1].events = 0;
        }
        client_fd1 = -1;
        client_fd2 = -1;
      }
    }
  }
  return 0;
}
