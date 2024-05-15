#include "buddy_allocator.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define FATAL(x)                                                               \
  do {                                                                         \
    fprintf(stderr, x);                                                        \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

////////////////////////////////
/// MATH FUNCTIONS
////////////////////////////////

static inline uint8_t uint64_log2(uint64_t v) {
  return 8 * (uint8_t)sizeof(uint64_t) - (uint8_t)__builtin_clzll(v);
}

static inline uint64_t uint64_pow2(uint8_t i) { return (uint64_t)1 << i; }

static inline bool uint64_is_power_of_2(uint64_t x) {
  return (x != 0) && ((x & (x - 1)) == 0);
}

static inline uint8_t uint8_max(uint8_t a, uint8_t b) {
  if (a > b) {
    return a;
  } else {
    return b;
  }
}

static inline uint8_t uint8_min(uint8_t a, uint8_t b) {
  if (a < b) {
    return a;
  } else {
    return b;
  }
}

////////////////////////////////
/// HEAP FUNCTIONS
////////////////////////////////

static inline uint8_t heap_level(uint64_t i) { return uint64_log2(i + 1); }

static inline uint64_t heap_size(uint8_t max_level) {
  return uint64_pow2(max_level + 1) - 1;
}

static inline uint64_t heap_parent(uint64_t i) { return (i - 1) / 2; }

static inline uint64_t heap_left(uint64_t i) { return 2 * i + 1; }

static inline uint64_t heap_right(uint64_t i) { return 2 * i + 2; }

static inline uint64_t heap_sibling(uint64_t i) {
  uint64_t parent = heap_parent(i);
  uint64_t parent_left = heap_left(parent);
  uint64_t parent_right = heap_right(parent);
  if (i == parent_left) {
    return parent_right;
  } else {
    return parent_left;
  }
}

// marks this block and any parent blocks as busy
static void mark_allocated(struct buddy_allocator_s *ba, uint64_t index) {
  ba->heap[index].smallest_free_level = BA_ALLOCATED;

  while (index != 0) {
    uint64_t parent = heap_parent(index);
    uint8_t sibling_free = ba->heap[heap_sibling(index)].smallest_free_level;

    if (ba->heap[parent].smallest_free_level == sibling_free) {
      // early exit
      break;
    } else {
      // otherwise the size of the parent's smallest free level is determined by
      // this blocks' smallest free level

      // the new smallest free level is the minimum of these two blocks
      uint8_t new_smallest_free_level =
          uint8_min(ba->heap[index].smallest_free_level, sibling_free);

      // set to BA_FILLED if one of them is not a valid level
      ba->heap[parent].smallest_free_level =
          new_smallest_free_level > BA_MAX_VALID_LEVEL
              ? BA_FILLED
              : new_smallest_free_level;

      // start processing the upper one
      index = parent;
    }
  }
}

// splits blocks to find an empty slot.
// Must ensure that space exists first, or will fail
static uint64_t acquire_empty_slot(struct buddy_allocator_s *ba,
                                   const uint8_t allocation_level) {
  if (allocation_level > ba->max_level) {
    FATAL("must have allocation level less than or equal to the max");
  }

  uint64_t index = 0;
  uint8_t level = 0;
  while (true) {
    if (allocation_level < ba->heap[index].smallest_free_level) {
      FATAL("must ensure that space exists before calling this function");
    }

    // this entire block is free
    if (ba->heap[index].smallest_free_level == level) {
      if (ba->heap[index].smallest_free_level == allocation_level) {
        // we found a free block that has the allocation level we desire and is
        // wholly unallocated!
        return index;
      } else if (level < ba->max_level) {
        // split block (the smallest level is now one of the children)
        ba->heap[index].smallest_free_level = level + 1;
        ba->heap[heap_left(index)].smallest_free_level = level + 1;
        ba->heap[heap_right(index)].smallest_free_level = level + 1;
      } else {
        FATAL("level == max_level but allocation_level > level");
      }
    }

    const uint64_t left_index = heap_left(index);
    const uint64_t right_index = heap_right(index);
    const uint8_t left_level = ba->heap[left_index].smallest_free_level;
    const uint8_t right_level = ba->heap[right_index].smallest_free_level;

    // pick the one with the larger level (smaller free block) so that we
    // preserve larger blocks for potential larger allocations
    if (left_level < right_level) {
      // if fits in the right level select that one
      if (allocation_level >= right_level) {
        index = right_index;
      } else {
        index = left_index;
      }
    } else {
      // if fits in the left level select that one
      if (allocation_level >= left_level) {
        index = left_index;
      } else {
        index = right_index;
      }
    }
    level++;
  }
}

// given an index into the heap, returns the index of the first page
static uint64_t get_first_page_index(struct buddy_allocator_s *ba, uint64_t i) {
  return i << (uint64_t)(ba->max_level - heap_level(i));
}

// TODO: need to fix this
static uint64_t get_index_from_page(struct buddy_allocator_s *ba, uint64_t i) {
  uint64_t block_index = 0;
  for (uint8_t level = 0; level <= ba->max_level; level++) {
    // check if this current block is the one
    if (ba->heap[block_index].smallest_free_level == BA_ALLOCATED) {
      return block_index;
    } else if (ba->heap[block_index].smallest_free_level == level) {
        // we hit a completely free block
        return 0xFFFF'FFFF'FFFF'FFFF;
    }

    // find whether the index would be in the left or right half.
    uint64_t is_right = i >> (ba->max_level - 1);

    if (is_right) {
      block_index = heap_right(i);
    } else {
      block_index = heap_left(i);
    }
  }
  return block_index;
}

static uint8_t get_max_level_from_n_pages(uint64_t n_pages) {
  if (n_pages == 0) {
    return 1;
  }
  return uint64_log2(n_pages) + !uint64_is_power_of_2(n_pages);
}

uint64_t buddy_get_heap_bytes(uint64_t n_pages) {
  uint8_t max_level = get_max_level_from_n_pages(n_pages);

  return heap_size(max_level) * sizeof(struct buddy_block_s);
}

struct buddy_allocator_s buddy_init(uint64_t n_pages, void *memory_location) {
  uint8_t max_level = get_max_level_from_n_pages(n_pages);

  struct buddy_allocator_s allocator = {.max_level = max_level,
                                        .heap = memory_location};

  // init the heap
  allocator.heap[0].smallest_free_level = 0;

  return allocator;
}

static void buddy_verify_recursive(struct buddy_allocator_s *ba, uint64_t i) {
  uint8_t level = heap_level(i);
  if (level == ba->max_level) {
    // the only valid values at this level are ba->max_level, BA_UNUSABLE, or
    // BA_ALLOCATED
    if (ba->heap[i].smallest_free_level == BA_UNUSABLE) {
      // unusable
    } else if (ba->heap[i].smallest_free_level == BA_ALLOCATED) {
      // allocated
    } else if (ba->heap[i].smallest_free_level == ba->max_level) {
      // free
    } else {
      FATAL("invalid smallest_free_level on bottom level block");
    }
  } else {
    const uint64_t left = heap_left(i);
    const uint64_t right = heap_right(i);
    if (ba->heap[i].smallest_free_level == BA_UNUSABLE) {
      // unsable
    } else if (ba->heap[i].smallest_free_level == BA_ALLOCATED) {
      // allocated
    } else if (ba->heap[i].smallest_free_level == BA_FILLED) {
      // filled

      // check children (they must be both be invalid)
      if (ba->heap[left].smallest_free_level > BA_MAX_VALID_LEVEL &&
          ba->heap[left].smallest_free_level > BA_MAX_VALID_LEVEL) {
        // ok
      } else {
        FATAL(
            "block claims to be filled, but at least one child has free space");
      }

      // verify children
      buddy_verify_recursive(ba, left);
      buddy_verify_recursive(ba, right);
    } else if (ba->heap[i].smallest_free_level < level) {
      // wrong!
      FATAL("block has a smaller level than should be possible at it's level");
    } else if (ba->heap[i].smallest_free_level == level) {
      // fully free block
    } else if (ba->heap[i].smallest_free_level > level &&
               ba->heap[i].smallest_free_level <= ba->max_level) {
      // split, at least one descendant is busy
      if (ba->heap[i].smallest_free_level ==
          uint8_min(ba->heap[left].smallest_free_level,
                    ba->heap[right].smallest_free_level)) {
        // ok
      } else {
        FATAL("block does not satisfy invariant that its smallest free "
              "level is the min of its children");
      }

      // if both of it's children are split and free, then that's an error
      if (ba->heap[left].smallest_free_level == level + 1 &&
          ba->heap[right].smallest_free_level == level + 1) {
        FATAL("two children should be merged");
      }

      // verify children
      buddy_verify_recursive(ba, left);
      buddy_verify_recursive(ba, right);
    } else {
      FATAL("block has a greater level than is permissible in this tree");
    }
  }
}

void buddy_verify(struct buddy_allocator_s *ba) {
  buddy_verify_recursive(ba, 0);
}

uint64_t buddy_allocate(struct buddy_allocator_s *ba,
                        unsigned _BitInt(24) allocator_id,
                        const uint8_t order) {
  if (order >= ba->max_level) {
    printf("order is too large");
    return 0xFFFF'FFFF'FFFF'FFFF;
  }

  const uint8_t level = ba->max_level - order;
  uint64_t index = acquire_empty_slot(ba, level);
  mark_allocated(ba, index);

  ba->heap[index].allocator_id = allocator_id;

  return get_first_page_index(ba, index);
}

void buddy_free(struct buddy_allocator_s *ba, unsigned _BitInt(24) allocator_id,
                uint64_t page_id) {
  // get the index of the allocation
}
