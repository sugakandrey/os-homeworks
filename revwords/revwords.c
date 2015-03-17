#include "helpers.h"
#include <stdio.h>

const size_t BUFFER_SIZE = 1024;
const char DELIM = ' ';

int main(void) {
    char buf[BUFFER_SIZE];
    
    while (1) {
      ssize_t bytes_read = read_until(STDIN_FILENO, buf, BUFFER_SIZE, DELIM);
      if (bytes_read < 0) {
	perror("Error happened while reading.\n");
	break;
      }
      int delimiter_found = (buf[bytes_read - 1] == ' ');
      for (size_t i = 0; i < (bytes_read - delimiter_found) / 2; i++) {
	char c = buf[i];
	buf[i] = buf[bytes_read - i - 1 - delimiter_found];
	buf[bytes_read - i - 1 - delimiter_found] = c;
      }
      write_(STDOUT_FILENO, buf, bytes_read);
      if (bytes_read == 0) {
	break;
      }
    }
    return 0;
}
