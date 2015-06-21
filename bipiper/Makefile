CFLAGS = -Wall -std=gnu99
DEPENDENCIES = ../lib
OS:=$(shell uname)

all: lib.so forking polling

clean:
	$(MAKE) -C $(DEPENDENCIES) clean
	rm -f forking

lib.so:
	$(MAKE) -C $(DEPENDENCIES)

forking: lib.so
ifeq ($(OS), Linux)
		$(CC) $(CFLAGS) -o $@ $@.c -I$(DEPENDENCIES) -L$(DEPENDENCIES) -Wl,-rpath=$(DEPENDENCIES) -lbufio -lhelpers
else
		$(CC) $(CFLAGS) -o $@ $@.c -I$(DEPENDENCIES) -L$(DEPENDENCIES) -lbufio -lhelpers
endif

polling: lib.so
ifeq ($(OS), Linux)
		$(CC) $(CFLAGS) -o $@ $@.c -I$(DEPENDENCIES) -L$(DEPENDENCIES) -WL,-rpath=$(DEPENDENCIES) -lbufio -lhelpers
else
		$(CC) $(CFLAGS) -o $@ $@.c -I$(DEPENDENCIES) -L$(DEPENDENCIES) -lbufio -lhelpers
endif
