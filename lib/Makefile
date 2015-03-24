CFLAGS = -Wall  -std=c99

all: libhelpers.so libbufio.so

clean:
	rm -f *.o
	rm -f *.so

libhelpers.so: helpers.o
	$(CC) -shared -o libhelpers.so helpers.o

libbufio.so: bufio.o
	$(CC) -shared -o libbufio.so bufio.o

helpers.o:
	$(CC) $(CFLAGS) -c -fPIC helpers.c
	
bufio.o:
	$(CC) $(CFLAGS) -c -fPIC bufio.c
