#include "heap.h"
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

void *heap_init(size_t size)
{
  static void *heap;

  if (heap == NULL)
    {
      heap = calloc(1, size);
    }

  assert(heap); 
  return heap;
}

void *new(void *heap, size_t size) 
{
  static void * frontier= NULL;
  void *location;

  if( frontier == NULL )
    frontier= heap;

  location= frontier;
  
  frontier += (size + (size%8)); // always aligned to 64b

  return location;
}

void *newa(void *heap, void *affinity, size_t size) 
{
  // Unsupported operation in heap-linear.c
  // Let's ignore the wish and defer to new
  return new( heap, size );
}

void *newr(void *heap, void *begin, void *end, size_t size) 
{
  // Unsupported operation in heap-linear.c
  // Let's ignore the directive and defer to new
  return new( heap, size );
}
