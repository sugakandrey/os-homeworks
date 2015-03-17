CFLAGS = -Wall -Werror -std=c99
DEPENDENCIES = ../lib

all: libhelpers.so cat

clean:
	$(MAKE) -C $(DEPENDENCIES) clean
	rm -f cat

libhelpers.so:
	$(MAKE) -C $(DEPENDENCIES)

cat: libhelpers.so
	$(CC) $(CFLAGS) -o $@ $@.c -I$(DEPENDENCIES) -L$(DEPENDENCIES) -lhelpers
