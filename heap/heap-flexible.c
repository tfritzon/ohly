#include "heap-flexible.h"

static inline mask_t *freemap_for_line(line_t *l)
{
  return l->heap->freemap + (l->start_address - l->heap->memory) / sizeof(mask_t); 
}

static inline mask_t *freemap_for_block(block_t *b)
{
  return b->heap->freemap + (b->start_address - b->heap->memory) / sizeof(mask_t); 
}

static inline void *h_end_address(heap_t *h)
{
  return h->memory + h->heap_size;
}

static inline void *h_start_address(heap_t *h)
{
  return h->memory;
}

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

  // Use calloc to allocate meta data -- no need to align with cache 
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
  heap->freemap = calloc(heap_size / sizeof(mask_t) / word_size, sizeof(mask_t)); 

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
  uint32_t offset = (p - h->memory) / h->word_size / sizeof(mask_t);
  h->freemap[offset] &= ~(1UL << ((p - h->memory) / h->word_size) % sizeof(mask_t));
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

void *lmalloc_general(const uint8_t word_size, 
		      void *mem_lower, 
		      void *mem_upper, 
		      mask_t mask, 
		      mask_t *freemap)
{
  // TODO: 
  // allocations never span freemap boundaries
  uint8_t shift = 0;

  while (mem_lower < mem_upper)
    {
      while (mask) // TODO: optimise this check
	{
	  if ((*freemap & mask) != mask)
	    {
	      return mem_lower + (shift * word_size);
	    }
	  else
	    {
	      mask = mask >> 1;
	      ++shift;
	    }
	}
      mem_lower += word_size;
      ++freemap;
    }

  return NULL;
}

void *rmalloc_general(const uint8_t word_size, 
		      void *mem_lower, 
		      void *mem_upper, 
		      mask_t mask, 
		      mask_t *freemap)
{
  // TODO: 
  // allocations never span freemap boundaries

  uint8_t shift = 0;
  
  while (mem_upper >= mem_lower)
    {
      while (mask)
	{
	  if ((*freemap & mask) != mask)
	    {
	      return mem_upper + (shift * word_size);
	    }
	  else
	    {
	      mask = mask << 1;
	      ++shift;
	    }
	}
      mem_upper -= word_size;
      ++freemap;
    }

  return NULL;
}

void *h_ptr_lmalloc(heap_t *h, void *p, size_t size)
{
  mask_t mask = ~(~0 >> (size / h->word_size)); 
  void *freemap_offsetted = h->freemap + (p - h->memory) / sizeof(mask_t);

  return lmalloc_general(h->word_size, 
			 p, 
			 h_end_address(h), 
			 mask, 
			 freemap_offsetted);
}

void *h_ptr_rmalloc(heap_t *h, void *p, size_t size)
{
  mask_t mask = ~(~0 >> (size / h->word_size)); 
  void *freemap_offsetted = h->freemap + (p - h->memory) / sizeof(mask_t);

  return rmalloc_general(h->word_size, 
			 p, 
			 h_start_address(h), 
			 mask, 
			 freemap_offsetted);
}

void *h_lmalloc(heap_t *h, size_t size)
{
  return h_ptr_lmalloc(h, h_start_address(h), size);
}

void *h_rmalloc(heap_t *h, size_t size)
{
  return h_ptr_rmalloc(h, h_end_address(h), size);
}

void *b_lmalloc(block_t *b, size_t size, bool ok_cross_block_boundary)
{
  return b_ptr_lmalloc(b, b->start_address, size, ok_cross_block_boundary);
}

void *b_rmalloc(block_t *b, size_t size, bool ok_cross_block_boundary)
{
  return b_ptr_lmalloc(b, b->end_address, size, ok_cross_block_boundary);
}

void *b_ptr_lmalloc(block_t *b, void *p, size_t size, bool ok_cross_block_boundary)
{
  void *end_address = ok_cross_block_boundary ? h_end_address(b->heap) : b->end_address;
  mask_t mask = ~(~0 >> (size / b->heap->word_size)); 

  return lmalloc_general(b->heap->word_size, 
			 p, 
			 end_address, 
			 mask, 
			 freemap_for_block(b));
}

void *b_ptr_rmalloc(block_t *b, void *p, size_t size, bool ok_cross_block_boundary)
{
  void *start_address = ok_cross_block_boundary ? h_start_address(b->heap) : b->start_address;
  mask_t mask = ~(~0 >> (size / b->heap->word_size)); 

  return rmalloc_general(b->heap->word_size, 
			 p, 
			 start_address, 
			 mask, 
			 freemap_for_block(b));
}

void *l_lmalloc(line_t *l, size_t size, bool ok_cross_line_boundary)
{
  return l_ptr_lmalloc(l, l->start_address, size, ok_cross_line_boundary);
}

void *l_rmalloc(line_t *l, size_t size, bool ok_cross_line_boundary)
{
  return l_ptr_rmalloc(l, l->end_address, size, ok_cross_line_boundary);
}

void *l_ptr_lmalloc(line_t *l, void *p, size_t size, bool ok_cross_line_boundary)
{
  void *end_address = ok_cross_line_boundary ? l->block->end_address : l->end_address;
  mask_t mask = ~(~0 >> (size / l->heap->word_size)); 

  return lmalloc_general(l->heap->word_size, 
			 p, 
			 end_address, 
			 mask, 
			 freemap_for_line(l));
}

void *l_ptr_rmalloc(line_t *l, void *p, size_t size, bool ok_cross_line_boundary)
{
  void *start_address = ok_cross_line_boundary ? l->block->start_address : l->start_address;
  mask_t mask = ~(~0 >> (size / l->heap->word_size)); 

  return rmalloc_general(l->heap->word_size, 
			 p, 
			 start_address, 
			 mask, 
			 freemap_for_line(l));
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
