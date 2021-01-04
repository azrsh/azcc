#include "testtool.h"
#include <stdio.h>

struct TupleInt2 {
  int x1;
  int x2;
};

struct TupleInt4 {
  int x1;
  int x2;
  int x3;
  int x4;
};

struct TupleInt5 {
  int x1;
  int x2;
  int x3;
  int x4;
  int x5;
};

void compare_tuple_int2(struct TupleInt2 instance1,
                        struct TupleInt2 instance2) {
  assert(instance1.x1, instance2.x1, "int2_1.x1 == int2_2.x1");
  assert(instance1.x2, instance2.x2, "int2_1.x2 == int2_2.x2");
  return;
}

void compare_tuple_int4(struct TupleInt4 instance1,
                        struct TupleInt4 instance2) {
  assert(instance1.x1, instance2.x1, "int4_1.x1 == int4_2.x1");
  assert(instance1.x2, instance2.x2, "int4_1.x2 == int4_2.x2");
  assert(instance1.x3, instance2.x3, "int4_1.x3 == int4_2.x3");
  assert(instance1.x4, instance2.x4, "int4_1.x4 == int4_2.x4");
  return;
}

void compare_tuple_int5(struct TupleInt5 instance1,
                        struct TupleInt5 instance2) {
  assert(instance1.x1, instance2.x1, "int5_1.x1 == int5_2.x1");
  assert(instance1.x2, instance2.x2, "int5_1.x2 == int5_2.x2");
  assert(instance1.x3, instance2.x3, "int5_1.x3 == int5_2.x3");
  assert(instance1.x4, instance2.x4, "int5_1.x4 == int5_2.x4");
  assert(instance1.x5, instance2.x5, "int5_1.x5 == int5_2.x5");
  return;
}

struct TupleInt2 test1(struct TupleInt2 instance) {
  return instance;
}

struct TupleInt4 test2(struct TupleInt4 instance) {
  return instance;
}

struct TupleInt5 test3(struct TupleInt5 instance) {
  return instance;
}

int main() {
  struct TupleInt5 a1, a2, a3, a4, a5;
  compare_tuple_int5(a1, a1);
  compare_tuple_int5(a2, a2);
  compare_tuple_int5(a3, a3);
  compare_tuple_int5(a4, a4);
  compare_tuple_int5(a5, a5);

  struct TupleInt2 b1, b2, b3, b4, b5, b6;
  b2 = test1(b1);
  compare_tuple_int2(b1, b2);
  b4 = test1(b3);
  compare_tuple_int2(b3, b4);
  b5 = test1(b6);
  compare_tuple_int2(b5, b6);

  struct TupleInt4 c1, c2, c3, c4, c5, c6;
  c2 = test2(c1);
  compare_tuple_int4(c1, c2);
  c4 = test2(c3);
  compare_tuple_int4(c3, c4);
  c5 = test2(c6);
  compare_tuple_int4(c5, c6);

  struct TupleInt5 d1, d2, d3, d4, d5, d6;
  d2 = test3(d1);
  compare_tuple_int5(d1, d2);
  d4 = test3(d3);
  compare_tuple_int5(d3, d4);
  d5 = test3(d6);
  compare_tuple_int5(d5, d6);

  printf("OK\n");
  return 0;
}
