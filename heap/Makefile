CC=gcc
#CFLAGS

all: linear

linear: heap.h heap-linear.c
	gcc -c heap-linear.c -o heap-linear.o
	ar rcs libheap-linear.a heap-linear.o
	cp libheap-linear.a ../lib

clean:
	rm *.o *.a *~
