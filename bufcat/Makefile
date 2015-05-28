CFLAGS = -Wall -Werror -std=c99
DEPENDENCIES = ../lib

all: libbufio.so bufcat

clean:
	$(MAKE) -C $(DEPENDENCIES) clean
	rm -f bufcat

libbufio.so:
	$(MAKE) -C $(DEPENDENCIES)

bufcat: libbufio.so
	$(CC) $(CFLAGS) -o $@ $@.c -I$(DEPENDENCIES) -L$(DEPENDENCIES) -Wl,-rpath=$(DEPENDENCIES) -lbufio

