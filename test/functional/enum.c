#include "testtool.h"
#include <stdio.h>

typedef enum { TEST_A, TEST_B } TestKind;
void test1() {
  TestKind kind = TEST_A;
  switch (kind) {
  case TEST_A:
    test_assert(0, 0, "expected pass");
    break;
  case TEST_B:
    test_assert(0, 1, "unexpected pass");
    break;
  }

  kind = TEST_B;
  switch (kind) {
  case TEST_A:
    test_assert(0, 1, "unexpected pass");
    break;
  case TEST_B:
    test_assert(0, 0, "expected pass");
    break;
  }

  printf("OK\n");
}

enum A { B, C };
int test2sub() {
  enum A b;
  return 174;
}
void test2() {
  test_assert(174, test2sub(), "emum A b;");
  printf("OK\n");
}

int main() {
  test1();
  test2();
  return 0;
}
