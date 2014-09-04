CC=gcc
#CFLAGS

all: linear

linear: heap.h heap_linear.c
	gcc -c heap_linear.c -o heap_linear.o
	ar rcs libheap_linear.a heap_linear.o
	cp libheap_linear.a ../lib

clean:
	rm *.o *.a *~
