#include "testtool.h"
#include <stdio.h>

typedef enum { TEST_A, TEST_B } TestKind;

int main() {
  TestKind kind = TEST_A;
  switch (kind) {
  case TEST_A:
    assert(0, 0, "expected pass");
    break;
  case TEST_B:
    assert(0, 1, "unexpected pass");
    break;
  }

  kind = TEST_B;
  switch (kind) {
  case TEST_A:
    assert(0, 1, "unexpected pass");
    break;
  case TEST_B:
    assert(0, 0, "expected pass");
    break;
  }

  printf("OK\n");
  return 0;
}
