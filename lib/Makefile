CFLAGS = -Wall -Werror -std=c99

all: libhelpers.so libbufio.so

clean:
	rm -f *.o
	rm -f *.dylib

libhelpers.so: helpers.o
	$(CC) -dynamiclib -install_name '$(CURDIR)/libhelpers.dylib' -o libhelpers.dylib helpers.o

libbufio.so: bufio.o
	$(CC) -dynamiclib -install_name '$(CURDIR)/libbufio.dylib' -o libbufio.dylib bufio.o

helpers.o: 
	$(CC) $(CFLAGS) -c -fPIC helpers.c

bufio.o: 
	$(CC) $(CFLAGS) -c -fPIC bufio.c
