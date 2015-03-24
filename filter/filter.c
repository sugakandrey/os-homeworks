#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const size_t BUFFER_SIZE = 4096;
const char DELIMITER = '\n';

int main(int argc, char * argv[]) {
  char buf[BUFFER_SIZE];
  char input_str[BUFFER_SIZE + 3];

  char ** args = malloc(sizeof(char *) * (argc + 1));
    
  for (int i = 0; i < argc - 1; i++) {
    args[i] = argv[i + 1];
  }
  args[argc - 1] = buf;
  args[argc] = NULL;
  char * file = args[0];

  while (1) {
    ssize_t bytes_read = read_until(STDIN_FILENO, buf, BUFFER_SIZE, DELIMITER);
    if (bytes_read == -1) {
      return 1;
    } else if (bytes_read == 0) {
      return 0;
    }
    int delimiter_found = (buf[bytes_read - 1] == DELIMITER);
    strncpy(input_str, buf, bytes_read - delimiter_found + 1);
    buf[bytes_read - delimiter_found] = '\0';
    if (spawn(file, args) == 0) {
      ssize_t bytes_written = write_(STDOUT_FILENO, input_str, bytes_read - delimiter_found + 1);
      if (bytes_written == -1) {
	return 1;
      }
    }
  }
  free(args);
  return 0;
}
