#include "testtool.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

bool compare_string(const char *p, const char *q) {
  if (strlen(p) != strlen(q))
    return false;

  return memcmp(p, q, strlen(q)) == 0;
}

bool test1() { return compare_string(__func__, "test1"); }
bool test2() { return compare_string(__func__, "test2"); }
bool test3() { return compare_string(__func__, "test3"); }

int main() {
  test_assert(1, test1(), "__func__ == \"test1\"");
  test_assert(1, test2(), "__func__ == \"test2\"");
  test_assert(1, test3(), "__func__ == \"test3\"");
  printf("OK\n");
  return 0;
}
