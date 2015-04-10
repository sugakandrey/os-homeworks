#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bufio.h"
#include "helpers.h"

const size_t BUFFER_SIZE = 4096;

int main(int argc, char * argv[]) {
  char input_str[BUFFER_SIZE + 3];

  char ** args = malloc(sizeof(char *) * (argc + 1));
  struct buf_t * buf = buf_new(BUFFER_SIZE);
  if (buf == NULL) {
    return 1;
  }
    
  for (int i = 0; i < argc - 1; i++) {
    args[i] = argv[i + 1];
  }
  args[argc - 1] = input_str;
  args[argc] = NULL;
  char * file = args[0];

  while (1) {
    ssize_t newline = buf_getline(STDIN_FILENO, buf, input_str);
    if (newline % 2 != 0) {
      continue;
    }
    if (newline == -1) {
      free(args);
      return 1;
    } else if (newline == 0) {
      free(args);
      return 0;
    }
    if (spawn(file, args) == 0) {
      input_str[newline] = '\n';
      ssize_t bytes_written = write_(STDOUT_FILENO, input_str, newline + 1);
      if (bytes_written == -1) {
      	free(args);
	return 2;
      }
    }
  }
  free(args);
  return 0;
}
