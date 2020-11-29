int assert(int, int, char *);
int printf();

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
  assert(126, instance.a, "instance.a");
  (*ptr).b = 1 + 2 * 3 + 8;
  assert(15, instance.b, "instance.b");
  return 0;
}

int test2() {
  struct TupleIntInt instance;
  struct TupleIntInt *ptr = &instance;
  (*ptr).a = 92 + 34;
  (*ptr).b = 1 + 2 * 3 + 8;
  assert(126, instance.a, "instance.a");
  assert(15, instance.b, "instance.b");
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
  assert(34, instance1.a, "instance1.a");
  assert(2184, instance2.a, "instance2.a");
  assert(134, instance3.a, "instance3.a");
  assert(3809, instance4.a, "instance4.a");
  assert(13789, instance1.b, "instance1.b");
  assert(133, instance2.b, "instance2.b");
  assert(643, instance3.b, "instance3.b");
  assert(1319, instance4.b, "instance4.b");
  return 0;
}

int test4() {
  struct TupleCharChar instance;
  struct TupleCharChar *ptr = &instance;
  (*ptr).a = 92 + 34;
  assert(126, instance.a, "instance.a");
  (*ptr).b = 1 + 2 * 3 + 8;
  assert(15, instance.b, "instance.b");
  return 0;
}

int test5() {
  struct TupleCharChar instance;
  struct TupleCharChar *ptr = &instance;
  (*ptr).a = 92 + 34;
  (*ptr).b = 1 + 2 * 3 + 8;
  assert(126, instance.a, "instance.a");
  assert(15, instance.b, "instance.b");
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
  assert(34, instance1.a, "instance1.a");
  assert(-120, instance2.a, "instance2.a");
  assert(-122, instance3.a, "instance3.a");
  assert(-31, instance4.a, "instance4.a");
  assert(-35, instance1.b, "instance1.b");
  assert(-123, instance2.b, "instance2.b");
  assert(-125, instance3.b, "instance3.b");
  assert(39, instance4.b, "instance4.b");
  return 0;
}

int test7() {
  struct TupleIntInt instance;
  struct TupleIntInt *ptr = &instance;
  ptr->a = 92 + 34;
  assert(126, instance.a, "instance.a");
  ptr->b = 1 + 2 * 3 + 8;
  assert(15, instance.b, "instance.b");
  return 0;
}

int test8() {
  struct TupleIntInt instance;
  struct TupleIntInt *ptr = &instance;
  ptr->a = 92 + 34;
  ptr->b = 1 + 2 * 3 + 8;
  assert(126, instance.a, "instance.a");
  assert(15, instance.b, "instance.b");
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
  assert(34, instance1.a, "instance1.a");
  assert(2184, instance2.a, "instance2.a");
  assert(134, instance3.a, "instance3.a");
  assert(3809, instance4.a, "instance4.a");
  assert(13789, instance1.b, "instance1.b");
  assert(133, instance2.b, "instance2.b");
  assert(643, instance3.b, "instance3.b");
  assert(1319, instance4.b, "instance4.b");
  return 0;
}

int test10() {
  struct TupleCharChar instance;
  struct TupleCharChar *ptr = &instance;
  ptr->a = 92 + 34;
  assert(126, instance.a, "instance.a");
  ptr->b = 1 + 2 * 3 + 8;
  assert(15, instance.b, "instance.b");
  return 0;
}

int test11() {
  struct TupleCharChar instance;
  struct TupleCharChar *ptr = &instance;
  ptr->a = 92 + 34;
  ptr->b = 1 + 2 * 3 + 8;
  assert(126, instance.a, "instance.a");
  assert(15, instance.b, "instance.b");
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
  assert(34, instance1.a, "instance1.a");
  assert(-120, instance2.a, "instance2.a");
  assert(-122, instance3.a, "instance3.a");
  assert(-31, instance4.a, "instance4.a");
  assert(-35, instance1.b, "instance1.b");
  assert(-123, instance2.b, "instance2.b");
  assert(-125, instance3.b, "instance3.b");
  assert(39, instance4.b, "instance4.b");
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
  assert(34, instance2.a, "instance2.a");
  assert(2184, instance2.b, "instance2.a");
  assert(-122, instance3.a, "instance3.a");
  assert(-31, instance3.b, "instance3.b");
  return 0;
}

/*int test14() {
  // 異なるポインタ型への代入を許可しない
  struct TupleIntInt instance1;
  struct TupleCharChar instance2;
  struct TupleIntInt *ptr1 = &instance2;
  struct TupleCharChar *ptr2 = &instance1;
}*/

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
  printf("OK\n");
  return 0;
}
