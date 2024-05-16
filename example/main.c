#include "buddy_allocator.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// test if 1 page works
static void test0() {
  uint64_t n_pages = 1;

  printf("n_pages %zu\n", n_pages);
  printf("n_bytes %zu\n", buddy_get_heap_bytes(n_pages));
  printf("sizeof buddy block %zu\n", sizeof(struct buddy_block_s));

  struct buddy_allocator_s ba =
      buddy_init(n_pages, malloc(buddy_get_heap_bytes(n_pages)));

  printf("verify\n");
  buddy_verify(&ba);

  // should fail
  printf("allocate (should fail)\n");
  uint64_t v0 = buddy_allocate(&ba, 0, 1);
  printf("result: %zu\n", v0);

  printf("verify\n");
  buddy_verify(&ba);

  printf("allocate (should succeed)\n");
  uint64_t v1 = buddy_allocate(&ba, 0, 0);
  printf("result: %zu\n", v1);

  printf("verify\n");
  buddy_verify(&ba);

  printf("allocate (should fail)\n");
  uint64_t v2 = buddy_allocate(&ba, 1, 0);
  printf("result: %zu\n", v2);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should fail)\n");
  buddy_free(&ba, 1, v1);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should succeed)\n");
  buddy_free(&ba, 0, v1);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should fail)\n");
  buddy_free(&ba, 0, v1);

  printf("verify\n");
  buddy_verify(&ba);

  // try allocating again
  printf("allocate (should succeed)\n");
  uint64_t v3 = buddy_allocate(&ba, 2, 0);
  printf("result: %zu\n", v3);

  printf("verify\n");
  buddy_verify(&ba);
}

// test if 2 pages works
static void test1() {
  uint64_t n_pages = 2;

  printf("n_pages %zu\n", n_pages);
  printf("n_bytes %zu\n", buddy_get_heap_bytes(n_pages));

  struct buddy_allocator_s ba =
      buddy_init(n_pages, malloc(buddy_get_heap_bytes(n_pages)));

  printf("verify\n");
  buddy_verify(&ba);

  printf("allocate (should fail)\n");
  uint64_t v0 = buddy_allocate(&ba, 0, 2);
  printf("result: %zu\n", v0);

  printf("verify\n");
  buddy_verify(&ba);

  printf("allocate (should succeed)\n");
  uint64_t v1 = buddy_allocate(&ba, 0, 0);
  printf("result: %zu\n", v1);

  printf("verify\n");
  buddy_verify(&ba);

  printf("allocate (should succeed)\n");
  uint64_t v2 = buddy_allocate(&ba, 1, 0);
  printf("result: %zu\n", v2);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should succeed)\n");
  buddy_free(&ba, 0, v1);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should succeed)\n");
  buddy_free(&ba, 1, v2);

  printf("verify\n");
  buddy_verify(&ba);

  // try allocating again
  printf("allocate\n");
  uint64_t v3 = buddy_allocate(&ba, 2, 1);
  printf("result: %zu\n", v3);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should succeed)\n");
  buddy_free(&ba, 2, v3);

  printf("verify\n");
  buddy_verify(&ba);
}

int main() {
  test0();
  test1();
}
