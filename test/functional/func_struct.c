#include "testtool.h"
#include <stdio.h>

struct TupleInt {
  int x1;
  int x2;
  int x3;
  int x4;
  int x5;
};

int test1(struct TupleInt instance1, struct TupleInt instance2) {
  assert(instance1.x1, instance2.x1, "instance1.x1 == instance2.x1");
  assert(instance1.x2, instance2.x2, "instance1.x2 == instance2.x2");
  assert(instance1.x3, instance2.x3, "instance1.x3 == instance2.x3");
  assert(instance1.x4, instance2.x4, "instance1.x4 == instance2.x4");
  assert(instance1.x5, instance2.x5, "instance1.x5 == instance2.x5");
  return 0;
}

int main() {
  struct TupleInt a1, a2, a3, a4, a5;
  test1(a1, a1);
  test1(a2, a2);
  test1(a3, a3);
  test1(a4, a4);
  test1(a5, a5);
  printf("OK\n");
  return 0;
}
