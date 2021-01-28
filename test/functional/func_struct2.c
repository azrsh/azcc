#include "testtool.h"
#include <stdio.h>

struct TupleInt2 {
  int x1;
  int x2;
};

void compare_tuple_int2(struct TupleInt2 instance1,
                        struct TupleInt2 instance2) {
  test_assert(instance1.x1, instance2.x1, "int2_1.x1 == int2_2.x1");
  test_assert(instance1.x2, instance2.x2, "int2_1.x2 == int2_2.x2");
  return;
}

struct TupleInt2 test1(struct TupleInt2 instance) {
  return instance;
}

int main() {
  struct TupleInt2 b1, b2;
  compare_tuple_int2(b1, b1);
  b2 = test1(b1);
  compare_tuple_int2(b1, b2);
  printf("OK\n");
  return 0;
}
