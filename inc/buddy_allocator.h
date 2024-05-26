#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include <stdint.h>

struct buddy_allocator_s;

uint64_t buddy_get_bytes(uint64_t n_pages);

// initializes a buddy allocator from uninitialized memory
void buddy_init(struct buddy_allocator_s *ba, uint64_t n_pages);

// marks a range of pages as unusable
void buddy_mark_unusable(struct buddy_allocator_s *ba, uint64_t min_page_id, uint64_t max_page_id);

// marks the buddy allocator as ready to use budy allocator must be initialized before this
void buddy_ready(struct buddy_allocator_s *ba);

// validate all the invariants of the buddy allocator heap. used for debugging
void buddy_verify(struct buddy_allocator_s *ba);

// returns status
uint64_t buddy_allocate(struct buddy_allocator_s *ba, uint64_t n_pages);

// accepts the page_id of the start of the allocation
void buddy_free(struct buddy_allocator_s *ba, uint64_t page_id);

#endif // BUDDY_ALLOCATOR_H
