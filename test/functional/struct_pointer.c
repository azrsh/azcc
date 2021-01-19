#include "testtool.h"
#include <stdio.h>
#include <stdlib.h>

struct TupleIntInt {
  int a;
  int b;
};

struct TupleCharChar {
  char a;
  char b;
};

struct TestCompound {
  struct TupleIntInt *a;
  struct TupleCharChar *b;
};

int test1() {
  struct TupleIntInt instance;
  struct TupleIntInt *ptr = &instance;
  (*ptr).a = 92 + 34;
  test_assert(126, instance.a, "instance.a");
  (*ptr).b = 1 + 2 * 3 + 8;
  test_assert(15, instance.b, "instance.b");
  return 0;
}

int test2() {
  struct TupleIntInt instance;
  struct TupleIntInt *ptr = &instance;
  (*ptr).a = 92 + 34;
  (*ptr).b = 1 + 2 * 3 + 8;
  test_assert(126, instance.a, "instance.a");
  test_assert(15, instance.b, "instance.b");
  return 0;
}

int test3() {
  struct TupleIntInt instance1;
  struct TupleIntInt instance2;
  struct TupleIntInt instance3;
  struct TupleIntInt instance4;
  struct TupleIntInt *ptr1 = &instance1;
  struct TupleIntInt *ptr2 = &instance2;
  struct TupleIntInt *ptr3 = &instance3;
  struct TupleIntInt *ptr4 = &instance4;
  (*ptr1).a = 34;
  (*ptr2).a = 2184;
  (*ptr3).a = 134;
  (*ptr4).a = 3809;
  (*ptr1).b = 13789;
  (*ptr2).b = 133;
  (*ptr3).b = 643;
  (*ptr4).b = 1319;
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
  struct TupleCharChar *ptr = &instance;
  (*ptr).a = 92 + 34;
  test_assert(126, instance.a, "instance.a");
  (*ptr).b = 1 + 2 * 3 + 8;
  test_assert(15, instance.b, "instance.b");
  return 0;
}

int test5() {
  struct TupleCharChar instance;
  struct TupleCharChar *ptr = &instance;
  (*ptr).a = 92 + 34;
  (*ptr).b = 1 + 2 * 3 + 8;
  test_assert(126, instance.a, "instance.a");
  test_assert(15, instance.b, "instance.b");
  return 0;
}

int test6() {
  struct TupleCharChar instance1;
  struct TupleCharChar instance2;
  struct TupleCharChar instance3;
  struct TupleCharChar instance4;
  struct TupleCharChar *ptr1 = &instance1;
  struct TupleCharChar *ptr2 = &instance2;
  struct TupleCharChar *ptr3 = &instance3;
  struct TupleCharChar *ptr4 = &instance4;
  (*ptr1).a = 34;
  (*ptr2).a = 2184;
  (*ptr3).a = 134;
  (*ptr4).a = 3809;
  (*ptr1).b = 13789;
  (*ptr2).b = 133;
  (*ptr3).b = 643;
  (*ptr4).b = 1319;
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

int test7() {
  struct TupleIntInt instance;
  struct TupleIntInt *ptr = &instance;
  ptr->a = 92 + 34;
  test_assert(126, instance.a, "instance.a");
  ptr->b = 1 + 2 * 3 + 8;
  test_assert(15, instance.b, "instance.b");
  return 0;
}

int test8() {
  struct TupleIntInt instance;
  struct TupleIntInt *ptr = &instance;
  ptr->a = 92 + 34;
  ptr->b = 1 + 2 * 3 + 8;
  test_assert(126, instance.a, "instance.a");
  test_assert(15, instance.b, "instance.b");
  return 0;
}

int test9() {
  struct TupleIntInt instance1;
  struct TupleIntInt instance2;
  struct TupleIntInt instance3;
  struct TupleIntInt instance4;
  struct TupleIntInt *ptr1 = &instance1;
  struct TupleIntInt *ptr2 = &instance2;
  struct TupleIntInt *ptr3 = &instance3;
  struct TupleIntInt *ptr4 = &instance4;
  ptr1->a = 34;
  ptr2->a = 2184;
  ptr3->a = 134;
  ptr4->a = 3809;
  ptr1->b = 13789;
  ptr2->b = 133;
  ptr3->b = 643;
  ptr4->b = 1319;
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

int test10() {
  struct TupleCharChar instance;
  struct TupleCharChar *ptr = &instance;
  ptr->a = 92 + 34;
  test_assert(126, instance.a, "instance.a");
  ptr->b = 1 + 2 * 3 + 8;
  test_assert(15, instance.b, "instance.b");
  return 0;
}

int test11() {
  struct TupleCharChar instance;
  struct TupleCharChar *ptr = &instance;
  ptr->a = 92 + 34;
  ptr->b = 1 + 2 * 3 + 8;
  test_assert(126, instance.a, "instance.a");
  test_assert(15, instance.b, "instance.b");
  return 0;
}

int test12() {
  struct TupleCharChar instance1;
  struct TupleCharChar instance2;
  struct TupleCharChar instance3;
  struct TupleCharChar instance4;
  struct TupleCharChar *ptr1 = &instance1;
  struct TupleCharChar *ptr2 = &instance2;
  struct TupleCharChar *ptr3 = &instance3;
  struct TupleCharChar *ptr4 = &instance4;
  ptr1->a = 34;
  ptr2->a = 2184;
  ptr3->a = 134;
  ptr4->a = 3809;
  ptr1->b = 13789;
  ptr2->b = 133;
  ptr3->b = 643;
  ptr4->b = 1319;
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

int test13() {
  struct TestCompound instance1;
  struct TupleIntInt instance2;
  struct TupleCharChar instance3;
  struct TestCompound *ptr = &instance1;
  ptr->a = &instance2;
  ptr->b = &instance3;
  ptr->a->a = 34;
  ptr->a->b = 2184;
  ptr->b->a = 134;
  ptr->b->b = 3809;
  test_assert(34, instance2.a, "instance2.a");
  test_assert(2184, instance2.b, "instance2.a");
  test_assert(-122, instance3.a, "instance3.a");
  test_assert(-31, instance3.b, "instance3.b");
  return 0;
}

/*int test14() {
  // 異なるポインタ型への代入を許可しない
  struct TupleIntInt instance1;
  struct TupleCharChar instance2;
  struct TupleIntInt *ptr1 = &instance2;
  struct TupleCharChar *ptr2 = &instance1;
}*/

struct TestCompound *test15sub() {
  struct TestCompound *result = calloc(1, sizeof(struct TestCompound));
  result->a = calloc(1, sizeof(struct TupleIntInt));
  result->b = calloc(1, sizeof(struct TupleCharChar));
  return result;
}
void test15() {
  int a1 = test15sub()->a->a;
  int b1 = test15sub()->a->b;
  char a2 = test15sub()->b->a;
  char b2 = test15sub()->b->b;
}

typedef struct Node Node;
struct Node {
  Node *lhs;
  Node *rhs;
};
void test16() {
  Node *node = calloc(1, sizeof(Node));
  node->lhs = calloc(1, sizeof(Node));
  node->lhs->lhs = calloc(1, sizeof(Node));
  node->lhs->rhs = calloc(1, sizeof(Node));
  node->rhs = calloc(1, sizeof(Node));
  node->rhs->lhs = calloc(1, sizeof(Node));
  node->rhs->rhs = calloc(1, sizeof(Node));
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
  test11();
  test12();
  test13();
  // test14();
  test15();
  test16();
  printf("OK\n");
  return 0;
}
