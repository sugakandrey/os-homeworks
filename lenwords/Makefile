CFLAGS =-Wall -Werror -std=c99
DEPENDENCIES = ../lib

all: libhelpers.so lenwords

clean:
	$(MAKE) -C $(DEPENDENCIES) clean
	rm -f lenwords

libhelpers.so:
	$(MAKE) -C $(DEPENDENCIES)

lenwords: libhelpers.so
	$(CC) $(CFLAGS) -o $@ $@.c -I$(DEPENDENCIES) -L$(DEPENDENCIES) -lhelpers
