CFLAGS = -Wall -std=c99
DEPENDENCIES = ../lib
OS:=$(shell uname)

all: lib.so simplesh

clean:
		$(MAKE) -C $(DEPENDENCIES) clean
			rm -f simplesh

lib.so:
		$(MAKE) -C $(DEPENDENCIES)

ifeq ($(OS), Linux)
simplesh: lib.so
		$(CC) $(CFLAGS) -o $@ $@.c -I$(DEPENDENCIES) -L$(DEPENDENCIES) -Wl,-rpath=$(DEPENDENCIES) -lbufio -lhelpers
else
simplesh: lib.so
		$(CC) $(CFLAGS) -o $@ $@.c -I$(DEPENDENCIES) -L$(DEPENDENCIES) -lbufio -lhelpers
endif
