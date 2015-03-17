CFLAGS =-Wall -Werror -std=c99
DEPENDENCIES = ../lib

all: libhelpers.so revwords

clean:
	$(MAKE) -C $(DEPENDENCIES) clean
	rm -f revwords

libhelpers.so:
	$(MAKE) -C $(DEPENDENCIES)

revwords: libhelpers.so
	$(CC) $(CFLAGS) -o $@ $@.c -I$(DEPENDENCIES) -L$(DEPENDENCIES) -lhelpers
