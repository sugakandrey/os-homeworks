CFLAGS = -Wall -Werror -std=c99
OS:=$(shell uname)

all: libhelpers.so libbufio.so

clean:
	rm -f *.o
	rm -f *.so
	rm -f *.dylib

libhelpers.so: helpers.o
ifeq ($(OS), Darwin)
	$(CC) -dynamiclib -install_name '$(CURDIR)/libhelpers.dylib' -o libhelpers.dylib helpers.o
else
	$(CC) -shared -o libhelpers.so helpers.o
endif

libbufio.so: bufio.o
ifeq ($(OS), Darwin)
	$(CC) -dynamiclib -install_name '$(CURDIR)/libbufio.dylib' -o libbufio.dylib bufio.o
else
	$(CC) -shared -o libbufio.so bufio.o
endif

helpers.o:
	$(CC) $(CFLAGS) -c -fPIC helpers.c

bufio.o:
	$(CC) $(CFLAGS) -c -fPIC bufio.c
