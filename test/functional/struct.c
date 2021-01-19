#include "testtool.h"
#include <stdio.h>

struct TupleIntInt {
  int a;
  int b;
};

struct TupleCharChar {
  char a;
  char b;
};

int test1() {
  struct TupleIntInt instance;
  instance.a = 92 + 34;
  test_assert(126, instance.a, "instance.a");
  instance.b = 1 + 2 * 3 + 8;
  test_assert(15, instance.b, "instance.b");
  return 0;
}

int test2() {
  struct TupleIntInt instance;
  instance.a = 92 + 34;
  instance.b = 1 + 2 * 3 + 8;
  test_assert(126, instance.a, "instance.a");
  test_assert(15, instance.b, "instance.b");
  return 0;
}

int test3() {
  struct TupleIntInt instance1;
  struct TupleIntInt instance2;
  struct TupleIntInt instance3;
  struct TupleIntInt instance4;
  instance1.a = 34;
  instance2.a = 2184;
  instance3.a = 134;
  instance4.a = 3809;
  instance1.b = 13789;
  instance2.b = 133;
  instance3.b = 643;
  instance4.b = 1319;
  test_assert(34, instance1.a, "instance1.a");
  test_assert(2184, instance2.a, "instance2.a");
  test_assert(134, instance3.a, "instance3.a");
  test_assert(3809, instance4.a, "instance4.a");
  test_assert(13789, instance1.b, "instance1.b");
  test_assert(133, instance2.b, "instance2.b");
  test_assert(643, instance3.b, "instance3.b");
  test_assert(1319, instance4.b, "instance4.b");
  return 0;
}

int test4() {
  struct TupleCharChar instance;
  instance.a = 92 + 34;
  test_assert(126, instance.a, "instance.a");
  instance.b = 1 + 2 * 3 + 8;
  test_assert(15, instance.b, "instance.b");
  return 0;
}

int test5() {
  struct TupleCharChar instance;
  instance.a = 92 + 34;
  instance.b = 1 + 2 * 3 + 8;
  test_assert(126, instance.a, "instance.a");
  test_assert(15, instance.b, "instance.b");
  return 0;
}

int test6() {
  struct TupleCharChar instance1;
  struct TupleCharChar instance2;
  struct TupleCharChar instance3;
  struct TupleCharChar instance4;
  instance1.a = 34;
  instance2.a = 2184;
  instance3.a = 134;
  instance4.a = 3809;
  instance1.b = 13789;
  instance2.b = 133;
  instance3.b = 643;
  instance4.b = 1319;
  test_assert(34, instance1.a, "instance1.a");
  test_assert(-120, instance2.a, "instance2.a");
  test_assert(-122, instance3.a, "instance3.a");
  test_assert(-31, instance4.a, "instance4.a");
  test_assert(-35, instance1.b, "instance1.b");
  test_assert(-123, instance2.b, "instance2.b");
  test_assert(-125, instance3.b, "instance3.b");
  test_assert(39, instance4.b, "instance4.b");
  return 0;
}

struct TupleCharCharIntChar {
  char a;
  char b;
  int c;
  char d;
};

struct TupleCharCharCharInt {
  char a;
  char b;
  char c;
  int d;
};

struct TupleCharIntCharIntChar {
  char a;
  int b;
  char c;
  int d;
  char e;
};

int test7() {
  test_assert(8, sizeof(struct TupleIntInt), "sizeof(struct TupleIntInt)");
  test_assert(2, sizeof(struct TupleCharChar), "sizeof(struct TupleCharChar)");
  test_assert(12, sizeof(struct TupleCharCharIntChar),
         "sizeof(struct TupleCharCharIntChar)");
  test_assert(8, sizeof(struct TupleCharCharCharInt),
         "sizeof(struct TupleCharCharCharInt)");
  test_assert(20, sizeof(struct TupleCharIntCharIntChar),
         "sizeof(struct TupleCharIntCharIntChar)");
  return 0;
}

int test8() {
  test_assert(4, _Alignof(struct TupleIntInt), "_Alignof(struct TupleIntInt)");
  test_assert(1, _Alignof(struct TupleCharChar), "_Alignof(struct TupleCharChar)");
  test_assert(4, _Alignof(struct TupleCharCharIntChar),
         "_Alignof(struct TupleCharCharIntChar)");
  test_assert(4, _Alignof(struct TupleCharCharCharInt),
         "_Alignof(struct TupleCharCharCharInt)");
  test_assert(4, _Alignof(struct TupleCharIntCharIntChar),
         "_Alignof(struct TupleCharIntCharIntChar)");
  return 0;
}

struct TupleCompound {
  struct TupleCharChar a;
  struct TupleCharCharIntChar b;
  struct TupleCharIntCharIntChar c;
};

int test9() {
  test_assert(4, _Alignof(struct TupleCompound), "_Alignof(struct TupleCompound)");
  test_assert(36, sizeof(struct TupleCompound), "sizeof(struct TupleCompound)");
  return 0;
}

void test10() {
  struct TupleIntInt instance1;
  struct TupleIntInt instance2;
  struct TupleIntInt instance3;
  struct TupleIntInt instance4;
  instance1.a = 34;
  instance2.a = 2184;
  instance3.a = 134;
  instance4.a = 3809;
  instance1.b = 13789;
  instance2.b = 133;
  instance3.b = 643;
  instance4.b = 1319;
  test_assert(0, instance1.a > instance1.b, "instance1.a > instance1.b");
  test_assert(1, instance1.a < instance1.b, "instance1.a < instance1.b");
  test_assert(1, instance2.a > instance2.b, "instance2.a > instance2.b");
  test_assert(0, instance2.a < instance2.b, "instance2.a < instance2.b");
  test_assert(0, instance3.a > instance3.b, "instance3.a > instance3.b");
  test_assert(1, instance3.a < instance3.b, "instance3.a < instance3.b");
  test_assert(1, instance4.a > instance4.b, "instance4.a > instance4.b");
  test_assert(0, instance4.a < instance4.b, "instance4.a < instance4.b");
}

int main() {
  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  test7();
  test8();
  test9();
  test10();
  printf("OK\n");
  return 0;
}
