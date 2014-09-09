CC=gcc -Wall -g
#CFLAGS

all: linear optimal flexible

optimal: heap.h heap-optimal.c
	$(CC) -c heap-optimal.c -o heap-optimal.o
	ar rcs libheap-optimal.a heap-optimal.o
	@mkdir -p ../lib
	cp libheap-optimal.a ../lib

flexible: heap.h heap-flexible.c
	$(CC) -c heap-flexible.c -o heap-flexible.o
	ar rcs libheap-flexible.a heap-flexible.o
	@mkdir -p ../lib
	cp libheap-flexible.a ../lib

linear: heap.h heap-linear.c
	$(CC) -c heap-linear.c -o heap-linear.o
	ar rcs libheap-linear.a heap-linear.o
	@mkdir -p ../lib
	cp libheap-linear.a ../lib

clean:
	rm *.o *.a *~
