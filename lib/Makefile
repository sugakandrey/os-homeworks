CFLAGS = -Wall -Werror -std=c99

all: libhelpers.so

clean:
	rm -f *.o
	rm -f *.dylib

libhelpers.so: helpers.o
	$(CC) -dynamiclib -install_name '$(CURDIR)/libhelpers.dylib' -o libhelpers.dylib helpers.o

helpers.o: helpers.c
	$(CC) $(CFLAGS) -c -fPIC helpers.c
