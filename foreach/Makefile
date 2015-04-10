CFLAGS = -Wall -Werror -std=c99
DEPENDENCIES = ../lib

all: lib.so foreach

clean:
	$(MAKE) -C $(DEPENDENCIES) clean
	rm -f foreach

lib.so:
	$(MAKE) -C $(DEPENDENCIES)

foreach: lib.so
	$(CC) $(CFLAGS) -o $@ $@.c -I$(DEPENDENCIES) -L$(DEPENDENCIES) -lbufio -lhelpers
