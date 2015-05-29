#ifndef _HELPERS_
#define _HELPERS_

#define _XOPEN_SOURCE
#include <unistd.h>
#include <stddef.h>

typedef struct execargs_t {
  char* file;
  char** args;
  size_t args_n;
} execargs_t;

execargs_t* execargs_new(char* file, char** args, size_t n);
void execargs_free(execargs_t* args);

ssize_t read_(int d, void* buf, size_t nbyte);
ssize_t write_(int d, const void* buf, size_t nbyte);
ssize_t read_until(int d, void* buf, size_t nbyte, char delimiter);
int spawn(const char* file, char* const argv []);
int exec(execargs_t* args);
int runpiped(execargs_t** programs, size_t n);

#endif
