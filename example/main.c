#include "buddy_allocator.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// test if 1 page works
static void test1() {
  printf("TEST 1\n");
  uint64_t n_pages = 1;

  struct buddy_allocator_s ba =
      buddy_init(n_pages, malloc(buddy_get_heap_bytes(n_pages)));
  buddy_ready(&ba);

  printf("verify\n");
  buddy_verify(&ba);

  printf("allocate (should fail, allocation of size 0)\n");
  uint64_t v0 = buddy_allocate(&ba, 0);
  printf("result: %zu\n", v0);

  printf(
      "allocate (should fail, allocation greater than could be supported)\n");
  uint64_t v1 = buddy_allocate(&ba, 2);
  printf("result: %zu\n", v1);

  printf("verify\n");
  buddy_verify(&ba);

  printf("allocate (should succeed)\n");
  uint64_t v2 = buddy_allocate(&ba, 1);
  printf("result: %zu\n", v2);

  printf("verify\n");
  buddy_verify(&ba);

  printf("allocate (should fail, not enough memory)\n");
  uint64_t v3 = buddy_allocate(&ba, 1);
  printf("result: %zu\n", v3);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should succeed)\n");
  buddy_free(&ba, v2);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should fail, no allocation at page)\n");
  buddy_free(&ba, v2);

  printf("verify\n");
  buddy_verify(&ba);

  // try allocating again
  printf("allocate (should succeed)\n");
  uint64_t v4 = buddy_allocate(&ba, 1);
  printf("result: %zu\n", v4);

  printf("verify\n");
  buddy_verify(&ba);

  free(ba.heap);
}

// test if 2 pages works
static void test2() {
  printf("TEST 2\n");
  uint64_t n_pages = 2;

  struct buddy_allocator_s ba =
      buddy_init(n_pages, malloc(buddy_get_heap_bytes(n_pages)));
  buddy_ready(&ba);

  printf("verify\n");
  buddy_verify(&ba);

  printf("allocate (should fail, allocation of size 0)\n");
  uint64_t v0 = buddy_allocate(&ba, 0);
  printf("result: %zu\n", v0);

  printf(
      "allocate (should fail, allocation greater than could be supported)\n");
  uint64_t v1 = buddy_allocate(&ba, 3);
  printf("result: %zu\n", v1);

  printf("verify\n");
  buddy_verify(&ba);

  printf("allocate (should succeed)\n");
  uint64_t v2 = buddy_allocate(&ba, 2);
  printf("result: %zu\n", v2);

  printf("verify\n");
  buddy_verify(&ba);

  printf("allocate (should fail, not enough memory)\n");
  uint64_t v3 = buddy_allocate(&ba, 1);
  printf("result: %zu\n", v3);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should succeed)\n");
  buddy_free(&ba, v2);

  printf("verify\n");
  buddy_verify(&ba);

  // try allocating again
  printf("allocate (should succeed)\n");
  uint64_t v4 = buddy_allocate(&ba, 1);
  printf("result: %zu\n", v4);

  // try allocating again
  printf("allocate (should succeed)\n");
  uint64_t v5 = buddy_allocate(&ba, 1);
  printf("result: %zu\n", v5);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should succeed)\n");
  buddy_free(&ba, v4);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should succeed)\n");
  buddy_free(&ba, v5);

  printf("verify\n");
  buddy_verify(&ba);

  printf("allocate (should succeed)\n");
  uint64_t v6 = buddy_allocate(&ba, 2);
  printf("result: %zu\n", v6);

  printf("verify\n");
  buddy_verify(&ba);

  free(ba.heap);
}

// test if 4 pages works
static void test4() {
  printf("TEST 4\n");
  uint64_t n_pages = 4;

  struct buddy_allocator_s ba =
      buddy_init(n_pages, malloc(buddy_get_heap_bytes(n_pages)));
  buddy_ready(&ba);

  printf("verify\n");
  buddy_verify(&ba);

  printf(
      "allocate (should fail, allocation greater than could be supported)\n");
  uint64_t v1 = buddy_allocate(&ba, 5);
  printf("result: %zu\n", v1);

  printf("verify\n");
  buddy_verify(&ba);

  printf("allocate v2 (should succeed)\n");
  uint64_t v2 = buddy_allocate(&ba, 4);
  printf("result: %zu\n", v2);

  printf("verify\n");
  buddy_verify(&ba);

  printf("allocate (should fail, not enough memory)\n");
  uint64_t v3 = buddy_allocate(&ba, 1);
  printf("result: %zu\n", v3);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free v2 (should succeed)\n");
  buddy_free(&ba, v2);

  printf("verify\n");
  buddy_verify(&ba);

  // try allocating again
  printf("allocate v4 (should succeed)\n");
  uint64_t v4 = buddy_allocate(&ba, 2);
  printf("result: %zu\n", v4);

  // try allocating again
  printf("allocate v5 (should succeed)\n");
  uint64_t v5 = buddy_allocate(&ba, 1);
  printf("result: %zu\n", v5);

  // try allocating again
  printf("allocate v6 (should succeed)\n");
  uint64_t v6 = buddy_allocate(&ba, 1);
  printf("result: %zu\n", v6);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should succeed)\n");
  buddy_free(&ba, v4);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should succeed)\n");
  buddy_free(&ba, v5);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should succeed)\n");
  buddy_free(&ba, v6);

  printf("verify\n");
  buddy_verify(&ba);

  free(ba.heap);
}

// test if 8 pages works
static void test8() {
  printf("TEST 8\n");
  uint64_t n_pages = 8;

  struct buddy_allocator_s ba =
      buddy_init(n_pages, malloc(buddy_get_heap_bytes(n_pages)));
  buddy_ready(&ba);

  printf("verify\n");
  buddy_verify(&ba);

  printf(
      "allocate (should fail, allocation greater than could be supported)\n");
  uint64_t v1 = buddy_allocate(&ba, 9);
  printf("result: %zu\n", v1);

  printf("verify\n");
  buddy_verify(&ba);

  printf("allocate v2 (should succeed)\n");
  uint64_t v2 = buddy_allocate(&ba, 5);
  printf("result: %zu\n", v2);

  printf("verify\n");
  buddy_verify(&ba);

  printf("allocate (should fail, not enough memory)\n");
  uint64_t v3 = buddy_allocate(&ba, 1);
  printf("result: %zu\n", v3);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free v2 (should succeed)\n");
  buddy_free(&ba, v2);

  printf("verify\n");
  buddy_verify(&ba);

  // try allocating again
  printf("allocate v4 (should succeed)\n");
  uint64_t v4 = buddy_allocate(&ba, 4);
  printf("result: %zu\n", v4);

  printf("verify\n");
  buddy_verify(&ba);

  // try allocating again
  printf("allocate v5 (should succeed)\n");
  uint64_t v5 = buddy_allocate(&ba, 2);
  printf("result: %zu\n", v5);

  printf("verify\n");
  buddy_verify(&ba);

  // try allocating again
  printf("allocate v6 (should succeed)\n");
  uint64_t v6 = buddy_allocate(&ba, 1);
  printf("result: %zu\n", v6);

  printf("verify\n");
  buddy_verify(&ba);

  // try allocating again
  printf("allocate v7 (should succeed)\n");
  uint64_t v7 = buddy_allocate(&ba, 1);
  printf("result: %zu\n", v7);

  printf("verify\n");
  buddy_verify(&ba);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should succeed)\n");
  buddy_free(&ba, v4);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should succeed)\n");
  buddy_free(&ba, v5);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should succeed)\n");
  buddy_free(&ba, v6);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should succeed)\n");
  buddy_free(&ba, v7);

  // do it in reverse order (shows there's no fragmentation)

  printf("verify\n");
  buddy_verify(&ba);

  // try allocating again
  printf("allocate v8 (should succeed)\n");
  uint64_t v8 = buddy_allocate(&ba, 1);
  printf("result: %zu\n", v4);

  printf("verify\n");
  buddy_verify(&ba);

  // try allocating again
  printf("allocate v9 (should succeed)\n");
  uint64_t v9 = buddy_allocate(&ba, 1);
  printf("result: %zu\n", v5);

  printf("verify\n");
  buddy_verify(&ba);

  // try allocating again
  printf("allocate vA (should succeed)\n");
  uint64_t vA = buddy_allocate(&ba, 2);
  printf("result: %zu\n", v6);

  printf("verify\n");
  buddy_verify(&ba);

  // try allocating again
  printf("allocate vB (should succeed)\n");
  uint64_t vB = buddy_allocate(&ba, 4);
  printf("result: %zu\n", v7);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should succeed)\n");
  buddy_free(&ba, v8);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should succeed)\n");
  buddy_free(&ba, v9);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should succeed)\n");
  buddy_free(&ba, vA);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should succeed)\n");
  buddy_free(&ba, vB);

  printf("verify\n");
  buddy_verify(&ba);

  free(ba.heap);
}


// test if 3 pages works
static void test3() {
  printf("TEST 3\n");
  uint64_t n_pages = 3;

  struct buddy_allocator_s ba =
      buddy_init(n_pages, malloc(buddy_get_heap_bytes(n_pages)));
  buddy_ready(&ba);

  printf("verify\n");
  buddy_verify(&ba);

  printf("allocate (should fail, allocation of size 0)\n");
  uint64_t v0 = buddy_allocate(&ba, 0);
  printf("result: %zu\n", v0);

  printf(
      "allocate (should fail, allocation greater than could be supported)\n");
  uint64_t v1 = buddy_allocate(&ba, 3);
  printf("result: %zu\n", v1);

  printf("verify\n");
  buddy_verify(&ba);

  printf("allocate (should succeed)\n");
  uint64_t v2 = buddy_allocate(&ba, 2);
  printf("result: %zu\n", v2);

  printf("verify\n");
  buddy_verify(&ba);

  printf("allocate (should succeed)\n");
  uint64_t v3 = buddy_allocate(&ba, 1);
  printf("result: %zu\n", v3);

  printf("verify\n");
  buddy_verify(&ba);

  printf("allocate (should fail)\n");
  uint64_t v4 = buddy_allocate(&ba, 1);
  printf("result: %zu\n", v4);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should succeed)\n");
  buddy_free(&ba, v2);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should succeed)\n");
  buddy_free(&ba, v3);

  printf("verify\n");
  buddy_verify(&ba);

  // try allocating again
  printf("allocate (should succeed)\n");
  uint64_t v5 = buddy_allocate(&ba, 1);
  printf("result: %zu\n", v5);

  printf("verify\n");
  buddy_verify(&ba);

  // try allocating again
  printf("allocate (should succeed)\n");
  uint64_t v6 = buddy_allocate(&ba, 1);
  printf("result: %zu\n", v6);

  printf("verify\n");
  buddy_verify(&ba);

  // try allocating again
  printf("allocate (should succeed)\n");
  uint64_t v7 = buddy_allocate(&ba, 1);
  printf("result: %zu\n", v7);


  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should succeed)\n");
  buddy_free(&ba, v5);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should succeed)\n");
  buddy_free(&ba, v6);

  printf("verify\n");
  buddy_verify(&ba);

  printf("free (should succeed)\n");
  buddy_free(&ba, v7);

  printf("verify\n");
  buddy_verify(&ba);


  free(ba.heap);
}


int main() {
  test1();
  test2();
  test4();
  test8();
  // now we test some of the features of marking blocks as unusable
  test3();
}
