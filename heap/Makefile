CC=gcc
#CFLAGS

all: linear optimal

optimal: heap.h heap-optimal.c
	gcc -c heap-optimal.c -o heap-optimal.o
	ar rcs libheap-optimal.a heap-optimal.o
	@mkdir -p ../lib
	cp libheap-optimal.a ../lib

linear: heap.h heap-linear.c
	gcc -c heap-linear.c -o heap-linear.o
	ar rcs libheap-linear.a heap-linear.o
	@mkdir -p ../lib
	cp libheap-linear.a ../lib

clean:
	rm *.o *.a *~
