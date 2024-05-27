#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include <stdint.h>

#define BUDDY_STATUS_SUCCESS 0
#define BUDDY_STATUS_INVAL 1
#define BUDDY_STATUS_NOMEM 2
#define BUDDY_STATUS_NO_SUCH_ALLOCATION 3

typedef uint64_t buddy_status_t;

struct buddy_allocator_s;

uint64_t buddy_get_bytes(uint64_t n_pages);

// initializes a buddy allocator from uninitialized memory
// ba: a pointer to memory at least buddy_get_bytes(n_pages) long
// n_pages: the number of pages to create an allocator for.
// page_size: the size of each page in bytes. must be a power of 2.
// offset: the offset of the range of memory controlled by the buddy allocator
void buddy_init(struct buddy_allocator_s *ba, uint64_t n_pages, uint64_t page_size, uint64_t offset);

// marks a range of pages as unusable
void buddy_mark_unusable(struct buddy_allocator_s *ba, uint64_t min_page_id, uint64_t max_page_id);

// marks the buddy allocator as ready to use budy allocator must be initialized before this
void buddy_ready(struct buddy_allocator_s *ba);

// validate all the invariants of the buddy allocator heap. used for debugging
void buddy_verify(struct buddy_allocator_s *ba);

// returns the status of the allocation. sets page_id
[[nodiscard("allocations may fail")]]
buddy_status_t buddy_page_alloc(struct buddy_allocator_s *ba, uint64_t n_pages, uint64_t* page_id);

// accepts the page_id of the start of the allocation
buddy_status_t buddy_page_free(struct buddy_allocator_s *ba, uint64_t page_id);

// returns the status of the allocation. sets mem
[[nodiscard("allocations may fail")]]
buddy_status_t buddy_mem_alloc(struct buddy_allocator_s *ba, uint64_t n_bytes, void** mem);

// accepts the pointer to the start of the allocation
buddy_status_t buddy_mem_free(struct buddy_allocator_s *ba, void* mem);

#endif // BUDDY_ALLOCATOR_H
