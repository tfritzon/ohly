#include <heap.h>
#include <assert.h>

void *heap_init(size_t size)
{
  static void *heap;

  if (heap == null)
    {
      heap = calloc(1, size);
    }

  assert(heap); 
  return heap;
}
