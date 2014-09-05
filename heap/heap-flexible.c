#include "heap-flexible.h"

// A line is a contiguous space of cache_line size bytes, which
// is aligned with a cache line.
struct line
{
  line_t *prev;
  line_t *next;
  void   *start_address;
  void   *end_address;
  block_t *block;
  heap_t *heap;
};

// A block is a contiguous space of cache size bytes, divided into
// a number of lines which all map to the same cache line
struct block
{
  block_t *prev;
  block_t *next;
  void    *start_address;
  void    *end_address;
  heap_t  *heap;
  line_t  *lines;
};

struct heap
{
  void    *memory;
  uint8_t *freemap; 
  block_t *blocks;
  line_t  *lines;
  assoc_t  associativity;
  uint32_t heap_size;
  uint16_t block_size;
  uint16_t cache_size;
  uint16_t no_blocks;
  uint16_t lines_per_block;
  uint8_t  cache_line_size;
  uint8_t  word_size;
};

static void line_setup(line_t *prev,
                       line_t *this,
                       line_t *next,
		       block_t *block)
{
  this->prev = prev;
  this->next = next;
  this->block = block;

  this->start_address = (prev ? prev->end_address + 1 : block->heap->memory);
  this->end_address   = this->start_address + block->heap->block_size - 1;
}

static void block_setup(block_t *prev,
			block_t *this,
			block_t *next,
			heap_t  *heap,
			line_t  *lines)
{
  this->prev = prev;
  this->next = next;
  this->heap = heap;

  this->start_address = prev ? prev->end_address + 1 : heap->memory;
  this->end_address   = this->start_address + heap->block_size - 1;

  this->lines = lines;

  line_t *prev_line = NULL;
  int i;

  for (i = 0; i < heap->lines_per_block; ++i)
    {
      line_t *this_line = &this->lines[i];
      line_t *next_line = (next == NULL && i == heap->lines_per_block - 1) ? NULL : &this->lines[i+1];

      line_setup(prev_line, this_line, next_line, this);

      prev_line = this_line;
    }
}

heap_t *heap_setup(uint32_t heap_size,
                   uint32_t cache_size,
                   uint8_t  cache_line_size,
		   uint8_t  word_size,
                   assoc_t  associativity)
{
  uint32_t no_blocks = cache_size / cache_line_size;
  uint32_t block_size = heap_size / no_blocks;

  assert(heap_size % cache_size == 0);
  assert(heap_size == no_blocks * block_size);

  heap_t *heap = calloc(1, sizeof(struct heap));

  heap->blocks          = calloc(no_blocks, sizeof(struct block));
  posix_memalign(&heap->memory, cache_line_size, heap_size);
  heap->associativity   = associativity;
  heap->heap_size       = heap_size;
  heap->block_size      = block_size;
  heap->cache_size      = cache_size;
  heap->no_blocks       = no_blocks;
  heap->lines_per_block = block_size / cache_line_size;
  heap->cache_line_size = cache_line_size;
  heap->word_size       = word_size;

  heap->lines = calloc(no_blocks * heap->lines_per_block, sizeof(struct line));
  heap->freemap = calloc(heap_size / sizeof(uint8_t) / word_size, sizeof(uint8_t)); 

  block_t *prev_block = NULL;
  int i;

  for (i = 1; i < no_blocks; ++i)
    {
      block_t *this_block = &heap->blocks[i];
      block_t *next_block = (i < no_blocks - 1) ? &heap->blocks[i+1] : NULL;

      block_setup(prev_block, this_block, next_block, heap, &heap->lines[i*heap->lines_per_block]);

      prev_block = this_block;
    }

  return heap;
}

// TODO: triple check 
void h_free(heap_t *h, void *p) 
{
  uint32_t offset = (p - h->memory) / h->word_size / sizeof(uint8_t);
  h->freemap[offset] &= ~(1UL << ((p - h->memory) / h->word_size) % sizeof(uint8_t));
}

block_t *h_block(heap_t *h, void *p)
{
  int i;
  for (i = 0; i < h->no_blocks; ++i)
    {
      block_t *b = &h->blocks[i];
      if (b->start_address <= p && p <= b->end_address) return b;
    }

  return NULL;
}

line_t *h_line(heap_t *h, void *p)
{
  int i;
  for (i = 0; i < h->no_blocks; ++i)
    {
      block_t *b = &h->blocks[i];
      if (b->start_address <= p && p <= b->end_address) return b_line(b, p);
    }

  return NULL;
}

line_t *b_line(block_t *b, void *p)
{
  int i;
  for (i = 0; i < b->heap->lines_per_block; ++i)
    {
      line_t *l = &b->lines[i];
      if (l->start_address <= p && p <= l->end_address) return l;
    }

  return NULL;
}

line_t *b_conflicting_line(line_t *l)
{
  if (l->next->block == l->block) 
    {
      return l->next;
    }
  else
    {
      return l->prev;
    }
}

void *h_ptr_lmalloc(heap_t *h, void *p, size_t size)
{
  // Search for the leftmost size / h->word_size unset bits in
  // h->freemap from the offset of p, and allocate at the
  // corresponding found offset
  return NULL;
}

void *h_ptr_rmalloc(heap_t *h, void *p, size_t size)
{
  // Search for the rightmost size / h->word_size unset bits in
  // h->freemap from the offset of p, and allocate at the
  // corresponding found offset
  return NULL;
}

// XXX: only supports size < 8 * word_size right now
void *h_lmalloc(heap_t *h, size_t size)
{
  uint8_t __mask = ~(~0 >> (size / h->word_size)); 
  uint8_t *freemap = h->freemap - 1;
  uint8_t shift = 0;
  while (*++freemap)
    {
      uint8_t mask = __mask;
      while (mask && (*freemap & mask) != mask) 
	{
	  ++shift;
	  mask = mask >> 1;
	}
      if (mask) break;
    }
  return h->memory + (freemap - h->freemap) * sizeof(uint8_t) + shift;
  
  // Search for the leftmost size / h->word_size unset bits in
  // h->freemap from the start of h->memory, and allocate at the
  // corresponding found offset
}

void *h_rmalloc(heap_t *h, size_t size)
{
  // Search for the rightmost size / h->word_size unset bits in
  // h->freemap from the start of h->memory, and allocate at the
  // corresponding found offset
  return NULL;
}

void *b_lmalloc(block_t *b, size_t size, bool ok_cross_block_boundary)
{
  // As h_lmalloc, but allocate is done inside a block, with the
  // opportunity to forbid allocs that would cross block
  // boundaries.
  return NULL;
}

void *b_rmalloc(block_t *b, size_t size, bool ok_cross_block_boundary)
{
  // See b_lmalloc
  return NULL;
}

void *b_ptr_lmalloc(block_t *b, void *p, size_t size)
{
  // See h_ptr_lmalloc
  return NULL;
}

void *b_ptr_rmalloc(block_t *b, void *p, size_t size)
{
  // See h_ptr_rmalloc
  return NULL;
}

void *l_lmalloc(line_t *l, size_t size, bool ok_cross_line_boundary)
{
  // See h_lmalloc, but allocate in a line with boundary checking
  return NULL;
}

void *l_rmalloc(line_t *l, size_t size, bool ok_cross_line_boundary)
{
  // See h_rmalloc, but allocate in a line with boundary checking
  return NULL;
}

void *l_ptr_lmalloc(line_t *l, void *p, size_t size)
{
  // See h_ptr_lmalloc
  return NULL;
}

void *l_ptr_rmalloc(line_t *l, void *p, size_t size)
{
  // See h_ptr_rmalloc
  return NULL;
}

block_t *b_ladjacent(block_t *b) 
{
  return b->prev;
}

block_t *b_radjacent(block_t *b)
{
  return b->next;
}

line_t *l_ladjacent(line_t *l)
{
  return l->prev;
}

line_t *l_radjacent(line_t *l)
{
  return l->next;
}
