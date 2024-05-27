#include "buddy_allocator.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// test if 1 page works
static void test1() {
  printf("TEST 1\n");
  uint64_t n_pages = 1;
  uint64_t page_size = 1;
  uint64_t offset = 0;

  struct buddy_allocator_s *ba = malloc(buddy_get_bytes(n_pages));
  buddy_init(ba, n_pages, page_size, offset);
  buddy_ready(ba);

  printf("verify\n");
  buddy_verify(ba);

  printf("allocate v0\n");
  uint64_t v0 = UINT64_MAX;
  buddy_status_t s0 = buddy_page_alloc(ba, 0, &v0);
  printf("result: %zu %zu\n", s0, v0);

  printf("verify\n");
  buddy_verify(ba);

  printf("free (should succeed)\n");
  buddy_page_free(ba, v0);

  printf(
      "allocate (should fail, allocation greater than could be supported)\n");
  uint64_t v1 = UINT64_MAX;
  buddy_status_t s1 = buddy_page_alloc(ba, 2, &v1);
  printf("result: %zu %zu\n", s1, v1);

  printf("verify\n");
  buddy_verify(ba);

  printf("allocate (should succeed)\n");
  uint64_t v2 = UINT64_MAX;
  buddy_status_t s2 = buddy_page_alloc(ba, 1, &v2);
  printf("result: %zu %zu\n", s2, v2);

  printf("verify\n");
  buddy_verify(ba);

  printf("allocate (should fail, not enough memory)\n");
  uint64_t v3 = UINT64_MAX;
  buddy_status_t s3 = buddy_page_alloc(ba, 1, &v3);
  printf("result: %zu %zu\n", s3, v3);

  printf("verify\n");
  buddy_verify(ba);

  printf("free (should succeed)\n");
  buddy_page_free(ba, v2);

  printf("verify\n");
  buddy_verify(ba);

  printf("free (should fail, no allocation at page)\n");
  buddy_page_free(ba, v2);

  printf("verify\n");
  buddy_verify(ba);

  // try allocating again
  printf("allocate (should succeed)\n");
  uint64_t v4 = UINT64_MAX;
  buddy_status_t s4 = buddy_page_alloc(ba, 1, &v4);
  printf("result: %zu %zu\n", s4, v4);

  printf("verify\n");
  buddy_verify(ba);

  free(ba);
}

// test if 2 pages works
static void test2() {
  printf("TEST 2\n");
  uint64_t n_pages = 2;
  uint64_t page_size = 1;
  uint64_t offset = 0;

  struct buddy_allocator_s *ba = malloc(buddy_get_bytes(n_pages));
  buddy_init(ba, n_pages, page_size, offset);
  buddy_ready(ba);

  printf("verify\n");
  buddy_verify(ba);

  printf("allocate v0\n");
  uint64_t v0 = UINT64_MAX;
  buddy_status_t s0 = buddy_page_alloc(ba, 0, &v0);
  printf("result: %zu %zu\n", s0, v0);

  printf("verify\n");
  buddy_verify(ba);

  printf("free (should succeed)\n");
  buddy_page_free(ba, v0);

  printf(
      "allocate (should fail, allocation greater than could be supported)\n");
  uint64_t v1 = UINT64_MAX;
  buddy_status_t s1 = buddy_page_alloc(ba, 3, &v1);
  printf("result: %zu %zu\n", s1, v1);

  printf("verify\n");
  buddy_verify(ba);

  printf("allocate (should succeed)\n");
  uint64_t v2 = UINT64_MAX;
  buddy_status_t s2 = buddy_page_alloc(ba, 2, &v2);
  printf("result: %zu %zu\n", s2, v2);

  printf("verify\n");
  buddy_verify(ba);

  printf("allocate (should fail, not enough memory)\n");
  uint64_t v3 = UINT64_MAX;
  buddy_status_t s3 = buddy_page_alloc(ba, 1, &v3);
  printf("result: %zu %zu\n", s3, v3);

  printf("verify\n");
  buddy_verify(ba);

  printf("free (should succeed)\n");
  buddy_page_free(ba, v2);

  printf("verify\n");
  buddy_verify(ba);

  // try allocating again
  printf("allocate (should succeed)\n");
  uint64_t v4 = UINT64_MAX;
  buddy_status_t s4 = buddy_page_alloc(ba, 1, &v4);
  printf("result: %zu %zu\n", s4, v4);

  // try allocating again
  printf("allocate (should succeed)\n");
  uint64_t v5 = UINT64_MAX;
  buddy_status_t s5 = buddy_page_alloc(ba, 1, &v5);
  printf("result: %zu %zu\n", s5, v5);

  printf("verify\n");
  buddy_verify(ba);

  printf("free (should succeed)\n");
  buddy_page_free(ba, v4);

  printf("verify\n");
  buddy_verify(ba);

  printf("free (should succeed)\n");
  buddy_page_free(ba, v5);

  printf("verify\n");
  buddy_verify(ba);

  printf("allocate (should succeed)\n");
  uint64_t v6 = UINT64_MAX;
  buddy_status_t s6 = buddy_page_alloc(ba, 2, &v6);
  printf("result: %zu %zu\n", s6, v6);

  printf("verify\n");
  buddy_verify(ba);

  free(ba);
}

// test if 4 pages works
static void test4() {
  printf("TEST 4\n");
  uint64_t n_pages = 4;
  uint64_t page_size = 1;
  uint64_t offset = 0;

  struct buddy_allocator_s *ba = malloc(buddy_get_bytes(n_pages));
  buddy_init(ba, n_pages, page_size, offset);
  buddy_ready(ba);


  printf("verify\n");
  buddy_verify(ba);

  printf(
      "allocate (should fail, allocation greater than could be supported)\n");
  uint64_t v1 = UINT64_MAX;
  buddy_status_t s1 = buddy_page_alloc(ba, 5, &v1);
  printf("result: %zu %zu\n", s1, v1);

  printf("verify\n");
  buddy_verify(ba);

  printf("allocate v2 (should succeed)\n");
  uint64_t v2 = UINT64_MAX;
  buddy_status_t s2 = buddy_page_alloc(ba, 4, &v2);
  printf("result: %zu %zu\n", s2, v2);

  printf("verify\n");
  buddy_verify(ba);

  printf("allocate (should fail, not enough memory)\n");
  uint64_t v3 = UINT64_MAX;
  buddy_status_t s3 = buddy_page_alloc(ba, 1, &v3);
  printf("result: %zu %zu\n", s3, v3);

  printf("verify\n");
  buddy_verify(ba);

  printf("free v2 (should succeed)\n");
  buddy_page_free(ba, v2);

  printf("verify\n");
  buddy_verify(ba);

  // try allocating again
  printf("allocate v4 (should succeed)\n");
  uint64_t v4 = UINT64_MAX;
  buddy_status_t s4 = buddy_page_alloc(ba, 2, &v4);
  printf("result: %zu %zu\n", s4, v4);

  // try allocating again
  printf("allocate v5 (should succeed)\n");
  uint64_t v5 = UINT64_MAX;
  buddy_status_t s5 = buddy_page_alloc(ba, 1, &v5);
  printf("result: %zu %zu\n", s5, v5);

  // try allocating again
  printf("allocate v6 (should succeed)\n");
  uint64_t v6 = UINT64_MAX;
  buddy_status_t s6 = buddy_page_alloc(ba, 1, &v6);
  printf("result: %zu %zu\n", s6, v6);

  printf("verify\n");
  buddy_verify(ba);

  printf("free (should succeed)\n");
  buddy_page_free(ba, v4);

  printf("verify\n");
  buddy_verify(ba);

  printf("free (should succeed)\n");
  buddy_page_free(ba, v5);

  printf("verify\n");
  buddy_verify(ba);

  printf("free (should succeed)\n");
  buddy_page_free(ba, v6);

  printf("verify\n");
  buddy_verify(ba);

  free(ba);
}

// test if 8 pages works
static void test8() {
  printf("TEST 8\n");
  uint64_t n_pages = 8;
  uint64_t page_size = 1;
  uint64_t offset = 0;

  struct buddy_allocator_s *ba = malloc(buddy_get_bytes(n_pages));
  buddy_init(ba, n_pages, page_size, offset);
  buddy_ready(ba);


  printf("verify\n");
  buddy_verify(ba);

  printf(
      "allocate (should fail, allocation greater than could be supported)\n");
  uint64_t v1 = UINT64_MAX;
  buddy_status_t s1 = buddy_page_alloc(ba, 9, &v1);
  printf("result: %zu %zu\n", s1, v1);

  printf("verify\n");
  buddy_verify(ba);

  printf("allocate v2 (should succeed)\n");
  uint64_t v2 = UINT64_MAX;
  buddy_status_t s2 = buddy_page_alloc(ba, 5, &v2);
  printf("result: %zu %zu\n", s2, v2);

  printf("verify\n");
  buddy_verify(ba);

  printf("allocate (should fail, not enough memory)\n");
  uint64_t v3 = UINT64_MAX;
  buddy_status_t s3 = buddy_page_alloc(ba, 1, &v3);
  printf("result: %zu %zu\n", s3, v3);

  printf("verify\n");
  buddy_verify(ba);

  printf("free v2 (should succeed)\n");
  buddy_page_free(ba, v2);

  printf("verify\n");
  buddy_verify(ba);

  // try allocating again
  printf("allocate v4 (should succeed)\n");
  uint64_t v4 = UINT64_MAX;
  buddy_status_t s4 = buddy_page_alloc(ba, 4, &v4);
  printf("result: %zu %zu\n", s4, v4);

  printf("verify\n");
  buddy_verify(ba);

  // try allocating again
  printf("allocate v5 (should succeed)\n");
  uint64_t v5 = UINT64_MAX;
  buddy_status_t s5 = buddy_page_alloc(ba, 2, &v5);
  printf("result: %zu %zu\n", s5, v5);

  printf("verify\n");
  buddy_verify(ba);

  // try allocating again
  printf("allocate v6 (should succeed)\n");
  uint64_t v6 = UINT64_MAX;
  buddy_status_t s6 = buddy_page_alloc(ba, 1, &v6);
  printf("result: %zu %zu\n", s6, v6);

  printf("verify\n");
  buddy_verify(ba);

  // try allocating again
  printf("allocate v7 (should succeed)\n");
  uint64_t v7 = UINT64_MAX;
  buddy_status_t s7 = buddy_page_alloc(ba, 1, &v7);
  printf("result: %zu %zu\n", s7, v7);

  printf("verify\n");
  buddy_verify(ba);

  printf("verify\n");
  buddy_verify(ba);

  printf("free (should succeed)\n");
  buddy_page_free(ba, v4);

  printf("verify\n");
  buddy_verify(ba);

  printf("free (should succeed)\n");
  buddy_page_free(ba, v5);

  printf("verify\n");
  buddy_verify(ba);

  printf("free (should succeed)\n");
  buddy_page_free(ba, v6);

  printf("verify\n");
  buddy_verify(ba);

  printf("free (should succeed)\n");
  buddy_page_free(ba, v7);

  // do it in reverse order (shows there's no fragmentation)

  printf("verify\n");
  buddy_verify(ba);

  // try allocating again
  printf("allocate v8 (should succeed)\n");
  uint64_t v8 = UINT64_MAX;
  buddy_status_t s8 = buddy_page_alloc(ba, 1, &v8);
  printf("result: %zu %zu\n", v8 , s8);

  printf("verify\n");
  buddy_verify(ba);

  // try allocating again
  printf("allocate v9 (should succeed)\n");
  uint64_t v9 = UINT64_MAX;
  buddy_status_t s9 = buddy_page_alloc(ba, 1, &v9);
  printf("result: %zu %zu\n", s9, v9);

  printf("verify\n");
  buddy_verify(ba);

  // try allocating again
  printf("allocate vA (should succeed)\n");
  uint64_t vA = UINT64_MAX;
  buddy_status_t sA = buddy_page_alloc(ba, 2, &vA);
  printf("result: %zu %zu\n", sA, vA);

  printf("verify\n");
  buddy_verify(ba);

  // try allocating again
  printf("allocate vB (should succeed)\n");
  uint64_t vB = UINT64_MAX;
  buddy_status_t sB = buddy_page_alloc(ba, 4, &vB);
  printf("result: %zu %zu\n", sB, vB);

  printf("verify\n");
  buddy_verify(ba);

  printf("free (should succeed)\n");
  buddy_page_free(ba, v8);

  printf("verify\n");
  buddy_verify(ba);

  printf("free (should succeed)\n");
  buddy_page_free(ba, v9);

  printf("verify\n");
  buddy_verify(ba);

  printf("free (should succeed)\n");
  buddy_page_free(ba, vA);

  printf("verify\n");
  buddy_verify(ba);

  printf("free (should succeed)\n");
  buddy_page_free(ba, vB);

  printf("verify\n");
  buddy_verify(ba);

  free(ba);
}

// test if 3 pages works
static void test3() {
  printf("TEST 3\n");
  uint64_t n_pages = 3;
  uint64_t page_size = 1;
  uint64_t offset = 0;

  struct buddy_allocator_s *ba = malloc(buddy_get_bytes(n_pages));
  buddy_init(ba, n_pages, page_size, offset);
  buddy_ready(ba);

  printf("verify\n");
  buddy_verify(ba);

  printf(
      "allocate (should fail, allocation greater than could be supported)\n");
  uint64_t v1 = UINT64_MAX;
  buddy_status_t s1 = buddy_page_alloc(ba, 3, &v1);
  printf("result: %zu %zu\n", s1, v1);

  printf("verify\n");
  buddy_verify(ba);

  printf("allocate v2 (should succeed)\n");
  uint64_t v2 = UINT64_MAX;
  buddy_status_t s2 = buddy_page_alloc(ba, 2, &v2);
  printf("result: %zu %zu\n", s2, v2);

  printf("verify\n");
  buddy_verify(ba);

  printf("allocate v3 (should succeed)\n");
  uint64_t v3 = UINT64_MAX;
  buddy_status_t s3 = buddy_page_alloc(ba, 1, &v3);
  printf("result: %zu %zu\n", s3, v3);

  printf("verify\n");
  buddy_verify(ba);

  printf("allocate v4 (should fail)\n");
  uint64_t v4 = UINT64_MAX;
  buddy_status_t s4 = buddy_page_alloc(ba, 1, &v4);
  printf("result: %zu %zu\n", s4, v4);

  printf("verify\n");
  buddy_verify(ba);

  printf("free v2 (should succeed)\n");
  buddy_page_free(ba, v2);

  printf("verify\n");
  buddy_verify(ba);

  printf("free v3 (should succeed)\n");
  buddy_page_free(ba, v3);

  printf("verify\n");
  buddy_verify(ba);

  printf("free v4 (should succeed)\n");
  buddy_page_free(ba, v4);

  printf("verify\n");
  buddy_verify(ba);

  // try allocating again
  printf("allocate (should succeed)\n");
  uint64_t v5 = UINT64_MAX;
  buddy_status_t s5 = buddy_page_alloc(ba, 1, &v5);
  printf("result: %zu %zu\n", s5, v5);

  printf("verify\n");
  buddy_verify(ba);

  // try allocating again
  printf("allocate (should succeed)\n");
  uint64_t v6 = UINT64_MAX;
  buddy_status_t s6 = buddy_page_alloc(ba, 1, &v6);
  printf("result: %zu %zu\n", s6, v6);

  printf("verify\n");
  buddy_verify(ba);

  // try allocating again
  printf("allocate (should succeed)\n");
  uint64_t v7 = UINT64_MAX;
  buddy_status_t s7 = buddy_page_alloc(ba, 1, &v7);
  printf("result: %zu %zu\n", s7, v7);

  printf("verify\n");
  buddy_verify(ba);

  printf("free (should succeed)\n");
  buddy_page_free(ba, v5);

  printf("verify\n");
  buddy_verify(ba);

  printf("free (should succeed)\n");
  buddy_page_free(ba, v6);

  printf("verify\n");
  buddy_verify(ba);

  printf("free (should succeed)\n");
  buddy_page_free(ba, v7);

  printf("verify\n");
  buddy_verify(ba);

  free(ba);
}

int main() {
  test1();
  test2();
  test4();
  test8();
  // now we test some of the features of marking blocks as unusable
  test3();
}
