#include "debug.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

[[noreturn]] void fatal(char *message) {
  fputs(message, stderr);
  exit(1);
}

void assert(bool h, char *message) {
  if (!h) {
    fatal(message);
  }
}

[[noreturn]] void fatal_s_u64_s(char *s1, uint64_t u1, char *s2) {
  fprintf(stderr, "%s%zu%s", s1, u1, s2);
  exit(1);
}

