#ifndef _ohly_heap_flexible_h_
#define _ohly_heap_flexible_h_

// ================================================================ 
// Minimal documentation
// ================================================================ 
// Memory is a space of contiguous blocks.
// A block is a contigous space divided into lines which all map to the same cache line.
// In a direct-mapped cache, each block maps to a single cache line.
// In a 2-way associative cache, each block maps to two different cache lines. 
// In a 4-way associative cache, each block maps to four different cache lines. 
// Only direct-mapped is "implemented". 

// You can ask the heap to return the block enclosing a pointer, and even
// the enclosing line. You can allocate directly in blocks and lines. 

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

typedef enum associativity assoc_t;
typedef struct block block_t;
typedef struct heap heap_t;
typedef struct line line_t;
typedef uint8_t mask_t;

enum associativity
  {
    DIRECT     = 1,
    TWO_WAY    = 2,
    FOUR_WAY   = 4,
    EIGHT_WAY  = 8,
    TWELVE_WAY = 12
  };


enum direction_t {
  LEFT,
  RIGHT
};

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
  mask_t *freemap; 
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



/** Setup a heap to allocate in */
heap_t *heap_setup(uint32_t heap_size, uint32_t l1_cache_size, uint8_t cache_line_size, uint8_t word_size, assoc_t associativity);

// ================================================================ 
// Get blocks and lines enclosing pointers
// ================================================================ 

/** Return a pointer to the block enclosing a pointer p */
block_t *h_block(heap_t *h, void *p);

/** Return a pointer to the line enclosing a pointer p */
line_t *h_line(heap_t *h, void *p);

/** Return a pointer to the line enclosing a pointer p */
line_t *b_line(block_t *b, void *p);

/** Return a pointer to a line in the same block */
line_t *b_conflicting_line(line_t *l);

/** Return a pointer to another block given a line */
block_t *b_non_conflicting_block(line_t *l);

/** Return a pointer to the left-adjacent block */
block_t *b_ladjacent(block_t *b);

/** Return a pointer to the left-adjacent block */
block_t *b_radjacent(block_t *b);

/** Return a pointer to the left-adjacent line */
line_t *l_ladjacent(line_t *l);

/** Return a pointer to the left-adjacent line */
line_t *l_radjacent(line_t *l);


// ================================================================ 
// Allocate in the heap
// ================================================================ 

/** Allocate on the lowest free address that can fit size */
void *h_lmalloc(heap_t *h, size_t size);

/** Allocate on the highest free address that can fit size */
void *h_rmalloc(heap_t *h, size_t size);

/** Allocate to the left of p in heap h */
void *h_ptr_lmalloc(heap_t *h, void *p, size_t size);

/** Allocate to the right of p in heap h */
void *h_ptr_rmalloc(heap_t *h, void *p, size_t size);


// ================================================================ 
// Allocate in a block
// ================================================================ 

/** Allocate on the lowest free address that can fit size */
void *b_lmalloc(block_t *b, size_t size, bool ok_cross_block_boundary);

/** Allocate on the lowest free address that can fit size */
void *b_rmalloc(block_t *b, size_t size, bool ok_cross_block_boundary);

/** Allocate to the left of p in block b */
void *b_ptr_lmalloc(block_t *b, void *p, size_t size, bool ok_cross_block_boundary);

/** Allocate to the right of p in block b */
void *b_ptr_rmalloc(block_t *b, void *p, size_t size, bool ok_cross_block_boundary);

// ================================================================ 
// Allocate in a line
// ================================================================ 

/** Allocate on the lowest free address that can fit size */
void *l_lmalloc(line_t *l, size_t size, bool ok_cross_line_boundary);

/** Allocate on the lowest free address that can fit size */
void *l_rmalloc(line_t *l, size_t size, bool ok_cross_line_boundary);

/** Allocate to the left of p in line l */
void *l_ptr_lmalloc(line_t *l, void *p, size_t size, bool ok_cross_line_boundary);

/** Allocate to the right of p in line l */
void *l_ptr_rmalloc(line_t *l, void *p, size_t size, bool ok_cross_line_boundary);


// ================================================================ 
// Deallocate memory
// ================================================================ 

void h_free(heap_t *h, void *p);

#endif
