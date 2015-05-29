CFLAGS = -Wall -std=gnu99
DEPENDENCIES = ../lib
OS:=$(shell uname)

all: lib.so simplesh

clean:
	$(MAKE) -C $(DEPENDENCIES) clean
	rm -f simplesh

lib.so:
	$(MAKE) -C $(DEPENDENCIES)

simplesh: lib.so
ifeq ($(OS), Linux)
		$(CC) $(CFLAGS) -o $@ $@.c -I$(DEPENDENCIES) -L$(DEPENDENCIES) -Wl,-rpath=$(DEPENDENCIES) -lbufio -lhelpers
else
		$(CC) $(CFLAGS) -o $@ $@.c -I$(DEPENDENCIES) -L$(DEPENDENCIES) -lbufio -lhelpers
endif
