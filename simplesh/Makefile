CFLAGS = -Wall -std=gnu99
DEPENDENCIES = ../lib

all: lib.so simplesh

clean:
	$(MAKE) -C $(DEPENDENCIES) clean
	rm -f simplesh

lib.so:
	$(MAKE) -C $(DEPENDENCIES)

simplesh: lib.so
	$(CC) $(CFLAGS) -o $@ $@.c -I$(DEPENDENCIES) -L$(DEPENDENCIES) -Wl,-rpath=$(DEPENDENCIES) -lbufio -lhelpers
