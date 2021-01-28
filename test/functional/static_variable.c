#include "testtool.h"
#include <stdio.h>

static int a;
static int b;
static int c = 12;

int test1() {
  static int a[2] = {0, 0};
  a[0] += 1;
  a[1] += 2;
  return a[0] + a[1];
}

int test2() {
  a = 1;
  return a;
}

int test3() {
  b = 2;
  return a + b;
}

int test4() { return c; }

int main() {
  for (int i = 0; i < 10; i++) {
    test_assert(
        (i + 1) * 3, test1(),
        "static int a[2] = {0, 0}; a[0] += 1; a[1] += 2; return a[0] + a[1];");
  }
  test_assert(1, test2(), "a = 1; return a;");
  test_assert(3, test3(), "b = 2; return a + b;");
  test_assert(12, test4(), "static int c = 12; int main(){ return c; }");
  printf("OK\n");
  return 0;
}
