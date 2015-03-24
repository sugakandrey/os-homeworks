CFLAGS =-Wall -Werror -std=c99
DEPENDENCIES = ../lib

all: libhelpers.so filter

clean:
	$(MAKE) -C $(DEPENDENCIES) clean
	rm -f filter

libhelpers.so:
	$(MAKE) -C $(DEPENDENCIES)

filter: libhelpers.so
	$(CC) $(CFLAGS) -o $@ $@.c -I$(DEPENDENCIES) -L$(DEPENDENCIES) -lhelpers
