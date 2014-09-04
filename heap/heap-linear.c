#include "heap.h"
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

void *new(void *frontier, size_t size) 
{
  void *location = frontier;
  
  // TODO: what about pointer alignment? 
  frontier += size;

  return location;
}

void *newa(void *heap, void *affinity, size_t size) 
{
  // Unsupported operation in heap-linear.c
  assert(false);
}

void *newr(void *heap, void *begin, void *end, size_t size) 
{
  // Unsupported operation in heap-linear.c
  assert(false);
}
