#include "helpers.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "bufio.h"

const size_t BUFFER_SIZE = 8192;
const char PIPE = '|';
const char SPACE = ' ';
static execargs_t** programs;

void sig_handler(int signal) {
  write_(STDOUT_FILENO, "\n", 1);
}

int is_delimiter(char c) {
  return (c == SPACE) || (c == PIPE) || (c == '\0') || (c == '\n');
}

size_t count_args(const char* buf, int from, int to) {
  size_t args_count = 0;
  for (int i = from; i < to - 1; i++) {
    if (!is_delimiter(buf[i]) && is_delimiter(buf[i + 1])) {
      args_count++;
    }
  }
  return args_count;
}

execargs_t* parse_command(const char* buf, const int from, const int to) {
  size_t args_count = count_args(buf, from, to);
  char** args = malloc(sizeof(char*) * (args_count + 1));
  int index = 0;
  int start = from;
  int end = to;
  for (int i = from; i < to - 1; i++) {
    if (!is_delimiter(buf[i]) && is_delimiter(buf[i + 1])) {
      end = i + 1;
      args[index++] = strndup(buf + start, end - start);
      i++;
      while (is_delimiter(buf[i])) i++;
      start = i;
    }
  }
  args[args_count] = NULL;
  return execargs_new(args[0], args, args_count);
}

void a(int sig) {}

int main(void) {
  struct sigaction sig_act;
  memset(&sig_act, '\0', sizeof(sig_act));
  sig_act.sa_handler = &sig_handler;
  sigaction(SIGINT, &sig_act, NULL);

  char buf[BUFFER_SIZE];
  buf_t* reader_buf = buf_new(BUFFER_SIZE);
  while (1) {
    if (write_(STDOUT_FILENO, "$", 1) == -1) {
      return EXIT_FAILURE;
    }
    ssize_t line_len = buf_getline(STDIN_FILENO, reader_buf, buf);
    if (line_len == -1) {
      continue;
    } else if (line_len == 0) {
      return EXIT_SUCCESS;
    }
    int programs_count = 1;
    for (ssize_t i = 0; i < line_len; i++) {
      if (buf[i] == PIPE) {
        programs_count++;
      }
    }
    programs = malloc(sizeof(execargs_t) * programs_count);
    int index = 0;
    int start = 0;
    int end = line_len;
    for (int i = 0; i < line_len; i++) {
      if (buf[i] == PIPE || i == line_len - 1) {
        end = i + 2;
        programs[index++] = parse_command(buf, start, end);
        start = end;
      }
    }
    runpiped(programs, programs_count);
  }
  for (execargs_t** it = programs; *it; it++) {
    execargs_free(*it);
  }
  free(programs);
  return EXIT_SUCCESS;
}

