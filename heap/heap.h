#ifndef HEAP_H
#define HEAP_H

#include <unistd.h>

/*
 * Initialize the heap to a certain size
 */
void * heap_init(size_t size);

/*
 * Allocate an object on the heap
 *
 * returns a pointer to the allocated object
 * returns NULL with errno set on failure
 */
void * new(void * heap, size_t size);

/*
 * Allocate an object on the heap as close to another object as possible
 *
 * returns a pointer to the allocated object
 * returns NULL with errno set on failure
 */
void * newa(void * heap, void * affinity, size_t size);

/*
 * Allocate an object strictly within a certain range of the heap
 *
 * returns a pointer to the allocated object
 * returns NULL with errno set on failure
 */
void * newr(void * heap, void * begin, void * end, size_t size);

#endif
