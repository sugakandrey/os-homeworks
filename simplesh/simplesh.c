#include <stdio.h>
#include "helpers.h"

const size_t BUFFER_SIZE = 8192;

void sig_handler(int signal) {
  write_(STDOUT_FILENO, "\n", 1);
}

int main(void) {
  char buf[BUFFER_SIZE];
  signal(SIGINT, sig_handler;)
  while (1) {
    write_(STDOUT_FILENO, "$", 1);
  }
}
