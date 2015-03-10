CFLAGS = -Wall -Werror -std=c99
DEPENDANCIES = ../lib

all: libhelpers.so cat

clean:
	$(MAKE) -C $(DEPENDANCIES) clean
	rm -f cat

libhelpers.so:
	$(MAKE) -C $(DEPENDANCIES)

cat: libhelpers.so
	$(CC) $(CLFAGS) -o $@ $@.c -I$(DEPENDANCIES) -L$(DEPENDANCIES) -Wl,-rpath $(DEPENDANCIES) -lhelpers

