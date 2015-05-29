CFLAGS = -Wall -std=c99
DEPENDENCIES = ../lib

all: lib.so simplesh

clean:
		$(MAKE) -C $(DEPENDENCIES) clean
			rm -f simplesh

lib.so:
		$(MAKE) -C $(DEPENDENCIES)

bufcat: lib.so
		$(CC) $(CFLAGS) -o $@ $@.c -I$(DEPENDENCIES) -L$(DEPENDENCIES) -Wl,-rpath=$(DEPENDENCIES) -lbufio -lhelpers
