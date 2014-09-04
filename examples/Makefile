
all: linkedlist-linear

linkedlist-linear: linkedlist.c
	gcc -g -o $@ $^ -I../heap -lheap-linear -L../lib

clean:
	rm -f *.o linkedlist-linear *~ *.s
