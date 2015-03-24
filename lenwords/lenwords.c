#include "helpers.h"
#include <stdio.h>

const size_t BUFFER_SIZE = 4096;
const size_t OUTPUT_SIZE = 15;
const char DELIM = ' ';

int main(void) {
    char buf[BUFFER_SIZE];
    char output[OUTPUT_SIZE];

    while (1) {
      ssize_t bytes_read = read_until(STDIN_FILENO, buf, BUFFER_SIZE, DELIM);
      if (bytes_read < 0) {
	return 1;
      }
      int written = 0;
      int delimiter_found = (buf[bytes_read - 1] == ' ');
      if (delimiter_found) {
	written = sprintf(output, "%d\n", (int) bytes_read - 1);
      } else if (bytes_read != 0) {
	written = sprintf(output, "%d\n", (int) bytes_read);
      } else {
	return 0;
      }
      ssize_t bytes_written = write_(STDOUT_FILENO, output, written);
      if (bytes_written == -1) {
	return 2;
      }
    }
    return 0;
}
