#include <assert.h>
#include "heap-flexible.h"

#define Kb * 1024
#define Mb * 1024 * 1024

void integrity_check(heap_t *h, size_t size) 
{
  assert(h->heap_size = size);
  // check that number of blocks is correct
  // check that all blocks have correct start and end address
  // check that all blocks are correctly linked

  // check that number of lines is correct
  // check that all lines have correct start and end address
  // check that all lines are correctly linked

  // check h_block, h_line are correct

  // check b_line, b_confliting_line, b_ladjacent and b_radjacent are correct

  // check l_ladjacent, and l_radjacent is correct
}

int main(void) 
{
  heap_t *h = heap_setup(1 Mb, 
			 64 Kb,
			 64,
			 8, 
			 DIRECT);

  integrity_check(h);

  return 0;
}
