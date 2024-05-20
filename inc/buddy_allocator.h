#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include <stdint.h>

#define BA_FILLED 255
#define BA_ALLOCATED 254
#define BA_UNUSABLE 253
#define BA_MAX_VALID_LEVEL 252

// DEFINITIONS:
// level: the root of a heap has level 0, it's children have level 1, etc

// order: order 0 is the smallest unit of memory that the buddy allocator can
// allocate, order 1 is 2x that size, order 2 4x, and so on.
// We can get level from order by doing n_levels - order

struct buddy_allocator_s {
  // the maximum level in the heap
  uint8_t max_level;
  // has (n_levels+1)^2 -1 entries forming a binary heap
  // Key properties:
  // for the n'th node, it's parent may be found at (n-1)/2
  // for the n'th node, it's left child may be found at 2*n + 1
  // for the n'th node, it's right child may be found at 2*n + 2
  // each entry has the following properties:
  // the smallest level of any of the children of this block which are empty
  // if BA_ALLOCATED, this is allocated to some process
  // if BA_UNUSABLE, this block should never be used or assigned
  // if BA_FILLED, both children are greater than ba_max_valid_level
  uint8_t *heap;
};

uint64_t buddy_get_heap_bytes(uint64_t n_pages);
struct buddy_allocator_s buddy_init(uint64_t n_pages, void *memory_location);

// validate all the invariants of the buddy allocator heap. used for debugging
void buddy_verify(struct buddy_allocator_s *ba);

// returns status
uint64_t buddy_allocate(struct buddy_allocator_s *ba, uint64_t n_pages);

// accepts the page_id of the start of the allocation
void buddy_free(struct buddy_allocator_s *ba, uint64_t page_id);

#endif // BUDDY_ALLOCATOR_H
