CFLAGS = -Wall -Werror -std=c99

all: libhelpers.so

clean:
	rm -f *.o
	rm -f *.so

libhelpers.so: helpers.o
	$(CC) -shared -o libhelpers.so helpers.o

helpers.o: helpers.c
	$(CC) $(CFLAGS) -c -fPIC helpers.c
