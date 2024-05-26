#include "buddy_allocator.h"

// TODO: implement https://arxiv.org/pdf/1804.03436

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "debug.h"

#define BA_FILLED 255
#define BA_ALLOCATED 254
#define BA_UNUSABLE 253
#define BA_MAX_VALID_LEVEL 252

#define BA_STATE_UNREADY 0
#define BA_STATE_READY 1

// DEFINITIONS:
// level: the root of a heap has level 0, it's children have level 1, etc

// order: order 0 is the smallest unit of memory that the buddy allocator can
// allocate, order 1 is 2x that size, order 2 4x, and so on.
// We can get level from order by doing n_levels - order

struct buddy_allocator_s {
  // buddy allocator state
  uint8_t state;
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
  uint8_t heap[];
};

////////////////////////////////
/// MATH FUNCTIONS
////////////////////////////////

static inline bool uint64_is_power_of_2(uint64_t x) {
  return __builtin_popcountll(x) == 1;
}

static inline uint8_t uint64_log2(uint64_t v) {
  return 8 * (uint8_t)sizeof(uint64_t) - (uint8_t)__builtin_clzll(v) - 1;
}

static inline uint8_t uint64_ceil_log2(uint64_t v) {
  return uint64_log2(v) + !uint64_is_power_of_2(v);
}

static inline uint64_t uint64_pow2(uint8_t i) { return (uint64_t)1 << i; }

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

// the level of a given index
static inline uint8_t heap_level(uint64_t i) { return uint64_log2(i + 1); }

// the size of the entire heap
static inline uint64_t heap_size(uint8_t max_level) {
  return uint64_pow2(max_level + 1) - 1;
}

// the parent index of the given index
static inline uint64_t heap_parent(uint64_t i) { return (i - 1) / 2; }

// the left child of the given index
static inline uint64_t heap_left(uint64_t i) { return 2 * i + 1; }

// the right child of the given index
static inline uint64_t heap_right(uint64_t i) { return 2 * i + 2; }

// the sibling of the given index
static inline uint64_t heap_sibling(uint64_t i) {
  if (i % 2 == 1) {
    // if index is odd, then we are a left child
    // add 1 to get the right child
    return i + 1;
  } else {
    // if index is even, then we are a right child
    // subtract 1 to get the left child
    return i - 1;
  }
}

// given two children , returns what the parent's
// should be
static uint8_t parent_free_level(const struct buddy_allocator_s *ba,
                                 uint8_t a_level, uint8_t b_level) {
  // the new smallest free level is the minimum of these two blocks
  uint8_t parent = uint8_min(a_level, b_level);

  if (parent > ba->max_level) {
    return BA_FILLED;
  } else {
    return parent;
  }
}

static void propagate(struct buddy_allocator_s *ba, uint64_t block_index) {
  // then update the on parent blocks
  while (block_index != 0) {
    uint64_t parent = heap_parent(block_index);
    uint8_t updated_parent_level = parent_free_level(
        ba, ba->heap[block_index], ba->heap[heap_sibling(block_index)]);

    // set the parent's level
    ba->heap[parent] = updated_parent_level;
    // start processing the upper one
    block_index = parent;
  }
}

// merges together free blocks starting at block index.
// returns the bock at which coalescing is not possible anymore
static uint64_t coalesce(struct buddy_allocator_s *ba, uint64_t block_index) {
  if (ba->heap[block_index] != heap_level(block_index)) {
    return block_index;
  }

  // try to merge blocks as much as we can
  while (block_index != 0) {
    uint8_t sibling_level = ba->heap[heap_sibling(block_index)];

    if (sibling_level == heap_level(block_index)) {
      uint64_t parent = heap_parent(block_index);
      ba->heap[parent] = heap_level(parent);
      block_index = parent;
    } else {
      break;
    }
  }
  return block_index;
}

// splits blocks to find an empty slot.
// Must ensure that space exists first, or will fail
static uint64_t acquire_empty_slot(struct buddy_allocator_s *ba,
                                   const uint8_t allocation_level) {

  assert(allocation_level <= ba->max_level,
         "must have allocation level less than or equal to the max");

  uint64_t index = 0;
  uint8_t level = 0;
  while (true) {
    assert(allocation_level >= ba->heap[index],
           "must ensure that space exists before calling this function");

    // this entire block is free
    if (ba->heap[index] == level) {
      if (ba->heap[index] == allocation_level) {
        // we found a free block that has the allocation level we desire and is
        // wholly unallocated!
        return index;
      } else {
        // split block (the smallest level is now one of the children)
        ba->heap[index] = level + 1;
        ba->heap[heap_left(index)] = level + 1;
        ba->heap[heap_right(index)] = level + 1;
      }
    }

    const uint64_t left_index = heap_left(index);
    const uint64_t right_index = heap_right(index);
    const uint8_t left_level = ba->heap[left_index];
    const uint8_t right_level = ba->heap[right_index];

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
static uint64_t
get_first_page_index_from_block_index(struct buddy_allocator_s *ba,
                                      uint64_t block_index) {
  return (block_index - heap_size(heap_level(block_index) - 1))
         << (ba->max_level - heap_level(block_index));
}

// given an index into the heap, returns the index of the last page
static uint64_t
get_last_page_index_from_block_index(struct buddy_allocator_s *ba,
                                     uint64_t block_index) {
  return ((block_index - heap_size(heap_level(block_index) - 1) + 1)
          << (ba->max_level - heap_level(block_index))) -
         1;
}

// given the index of a page, gets the allocation it belongs to
static uint64_t get_block_index_from_page_index(struct buddy_allocator_s *ba,
                                                uint64_t page_id) {
  uint64_t block_index = 0;
  for (uint8_t level = 0; level <= ba->max_level; level++) {
    // check if this current block is the one
    if (ba->heap[block_index] == BA_ALLOCATED) {
      return block_index;
    } else if (ba->heap[block_index] == level) {
      // we hit a completely free block
      return UINT64_MAX;
    } else if (ba->heap[block_index] == BA_UNUSABLE) {
      // we hit an unusable block
      return UINT64_MAX;
    }

    if (page_id >=
        get_first_page_index_from_block_index(ba, heap_right(block_index))) {
      block_index = heap_right(block_index);
    } else {
      block_index = heap_left(block_index);
    }
  }
  return block_index;
}

// gets the necessary number of bytes to construct the buddy allocator heap
uint64_t buddy_get_bytes(uint64_t n_pages) {
  assert(n_pages != 0, "n_pages must not be 0");

  uint8_t max_level = uint64_ceil_log2(n_pages);
  return sizeof(struct buddy_allocator_s) + heap_size(max_level);
}

void buddy_init(struct buddy_allocator_s *ba, uint64_t n_pages) {
  assert(n_pages != 0, "n_pages must not be 0");

  ba->state = BA_STATE_UNREADY;
  ba->max_level = uint64_ceil_log2(n_pages);

  uint64_t offset = 0;
  if (ba->max_level > 0) {
    offset = heap_size(ba->max_level - 1);
  }

  // init the bottom level of the heap
  for (uint64_t i = offset; i < offset + n_pages; i++) {
    ba->heap[i] = ba->max_level;
  }
  for (uint64_t i = n_pages + offset; i < heap_size(ba->max_level); i++) {
    ba->heap[i] = BA_UNUSABLE;
  }
}

void buddy_mark_unusable(struct buddy_allocator_s *ba, uint64_t min_page_id,
                         uint64_t max_page_id) {
  assert(ba->state == BA_STATE_UNREADY,
         "allocator state is ready (should be unready)\n");
  for (uint64_t i = min_page_id; i <= max_page_id; i++) {
    ba->heap[i + uint64_pow2(ba->max_level - 1)] = BA_UNUSABLE;
  }
}

void buddy_ready(struct buddy_allocator_s *ba) {
  if (ba->max_level > 0) {
    // walk backwards in the heap
    // start from the last block of the penultimate layer
    // compute the correct free level of the block
    // due to the way the heap is laid out, we are sure always to initialize
    // children before parents
    for (int64_t block_index = (int64_t)heap_size(ba->max_level - 1) - 1;
         block_index >= 0; block_index--) {
      uint64_t bi = (uint64_t)block_index;
      uint8_t level = heap_level(bi);
      uint8_t lv = ba->heap[heap_left(bi)];
      uint8_t rv = ba->heap[heap_right(bi)];

      if (lv == BA_UNUSABLE && rv == BA_UNUSABLE) {
        ba->heap[bi] = BA_UNUSABLE;
      } else if (lv == level + 1 && rv == level + 1) {
        ba->heap[bi] = heap_level(bi);
      } else {
        ba->heap[bi] = uint8_min(lv, rv);
      }
    }
  }
  ba->state = BA_STATE_READY;
}

static void buddy_verify_recursive(struct buddy_allocator_s *ba, uint64_t i) {
  assert(ba->state == BA_STATE_READY, "must be ready to be verified");

  uint8_t level = heap_level(i);
  if (level == ba->max_level) {
    // the only valid values at this level are ba->max_level, BA_UNUSABLE, or
    // BA_ALLOCATED
    if (ba->heap[i] == BA_UNUSABLE) {
      // unusable
    } else if (ba->heap[i] == BA_ALLOCATED) {
      // allocated
    } else if (ba->heap[i] == ba->max_level) {
      // free
    } else {
      fatal_s_u64_s("block ", i,
                    " has an invalid value for a bottom level block\n");
    }
  } else {
    const uint64_t left = heap_left(i);
    const uint64_t right = heap_right(i);
    if (ba->heap[i] == BA_UNUSABLE) {
      // unsable
    } else if (ba->heap[i] == BA_ALLOCATED) {
      // allocated
    } else if (ba->heap[i] == BA_FILLED) {
      // filled

      // check children (they must be both be invalid)
      if (ba->heap[left] > BA_MAX_VALID_LEVEL &&
          ba->heap[right] > BA_MAX_VALID_LEVEL) {
        // ok
      } else {
        fatal_s_u64_s(
            "block ", i,
            "claims to be filled, but at least one child has free space\n");
      }

      // verify children
      buddy_verify_recursive(ba, left);
      buddy_verify_recursive(ba, right);
    } else if (ba->heap[i] < level) {
      // wrong!
      fatal_s_u64_s("block ", i,
                    " has a smaller level than should be possible at "
                    "it's level\n");
    } else if (ba->heap[i] == level) {
      // fully free block
    } else if (ba->heap[i] > level && ba->heap[i] <= ba->max_level) {
      // split, at least one descendant is busy
      if (ba->heap[i] == uint8_min(ba->heap[left], ba->heap[right])) {
        // ok
      } else {

        fatal_s_u64_s("block ", i,
                      " does not satisfy invariant that its smallest free "
                      "level is the min of its children\n");
      }

      // if both of it's children are split and free, then that's an error
      if (ba->heap[left] == level + 1 && ba->heap[right] == level + 1) {
        fatal_s_u64_s("block ", i, " two children should be merged\n");
      }

      // verify children
      buddy_verify_recursive(ba, left);
      buddy_verify_recursive(ba, right);
    } else {
      fatal_s_u64_s("block ", i,
                    " has a greater level than is permissible in this tree\n");
    }
  }
}

void buddy_verify(struct buddy_allocator_s *ba) {

  for (uint64_t z = 0; z < heap_size(ba->max_level); z++) {
    printf("%u ", ba->heap[z]);
  }
  printf("\n");

  buddy_verify_recursive(ba, 0);
}

uint64_t buddy_allocate(struct buddy_allocator_s *ba, const uint64_t n_pages) {
  assert(ba->state == BA_STATE_READY, "allocator state is not ready\n");

  if (n_pages == 0 || n_pages > uint64_pow2(ba->max_level)) {
    return UINT64_MAX;
  }

  const uint8_t allocation_level = ba->max_level - uint64_ceil_log2(n_pages);

  if (allocation_level < ba->heap[0]) {
    return UINT64_MAX;
  }

  // split blocks to get a slot of the correct size
  const uint64_t block_index = acquire_empty_slot(ba, allocation_level);

  // mark this block as allocated and update parent blocks
  ba->heap[block_index] = BA_ALLOCATED;
  // update parent blocks
  propagate(ba, block_index);

  return get_first_page_index_from_block_index(ba, block_index);
}

void buddy_free(struct buddy_allocator_s *ba, uint64_t page_id) {
  assert(ba->state == BA_STATE_READY, "allocator state is not ready\n");

  const uint64_t block_index = get_block_index_from_page_index(ba, page_id);
  if (block_index == UINT64_MAX) {
    return;
  }

  // mark block as free
  ba->heap[block_index] = heap_level(block_index);
  // coalesce blocks starting from that point
  const uint64_t coalesced_block_index = coalesce(ba, block_index);
  // then update free space on the parent blocks
  propagate(ba, coalesced_block_index);
}
