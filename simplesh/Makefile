CFLAGS =-Wall -Werror -std=c99
DEPENDENCIES = ../lib

all: libhelpers.so simplesh

clean:
	$(MAKE) -C $(DEPENDENCIES) clean
	rm -f simplesh

libhelpers.so:
	$(MAKE) -C $(DEPENDENCIES)

simplesh: libhelpers.so
	$(CC) $(CFLAGS) -o $@ $@.c -I$(DEPENDENCIES) -L$(DEPENDENCIES) -lhelpers
