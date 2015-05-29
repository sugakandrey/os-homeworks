CFLAGS = -Wall -Werror -std=c99
OS:=$(shell uname)

all: libhelpers.so libbufio.so

clean:
	rm -f *.o
	rm -f *.so

ifeq ($(OS), Darwin)
libhelpers.so: helpers.o
	$(CC) -shared -o libhelpers.so helpers.o

libbufio.so: bufio.o
	$(CC) -dynamiclib -install_name '$(CURDIR)/libbufio.dylib' -o libbufio.dylib bufio.o
else
ibhelpers.so: helpers.o
	$(CC) -shared -o libhelpers.so helpers.o

libbufio.so: bufio.o
	$(CC) -shared -o libbufio.so bufio.o
endif

helpers.o:
	$(CC) $(CFLAGS) -c -fPIC helpers.c

bufio.o:
	$(CC) $(CFLAGS) -c -fPIC bufio.c
