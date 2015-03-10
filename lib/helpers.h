#ifndef _HELPERS_
#define _HELPERS_

#include <unistd.h>
#include <stddef.h>

ssize_t read_(int d, void *buf, size_t nbyte);
ssize_t write_(int d, const void *buf, size_t nbyte);

#endif
