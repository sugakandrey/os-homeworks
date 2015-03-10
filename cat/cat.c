#include "helpers.h"
#include <stdio.h>

const size_t BUFFER_SIZE = 1024;

int main(void) {
    char buf[BUFFER_SIZE];
    
    while (1) {
        ssize_t bytes_read = read_(STDIN_FILENO, buf, BUFFER_SIZE);
        if (bytes_read < 0) {
           perror("Error happened while reading.\n");
           break;
        }
        write_(STDOUT_FILENO, buf, bytes_read);
        if (bytes_read == 0) {
            break;
        }
    }
    return 0;
}
