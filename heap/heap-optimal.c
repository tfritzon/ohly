#include "heap.h"
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

typedef unsigned long *bitvec_t;

// Pointer to start of memory
void          *memory;

// Bitmap w/ 1 set for allocated buckets, 0 for free
bitvec_t      *allocated;

// Size of each bucket in bytes
unsigned short bucket_size; 

void *heap_init(size_t size)
{
  return NULL;
}

void *new(void *frontier, size_t size) 
{
  return NULL;
}

void *newa(void *heap, void *affinity, size_t size) 
{
  return NULL;
}

void *newr(void *heap, void *begin, void *end, size_t size) 
{
  return NULL;
}
