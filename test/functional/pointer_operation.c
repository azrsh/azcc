int test_assert(int, int, char *);
int printf();
char alloc4(int **p, int v1, int v2, int v3, int v4);

int test1() {
  int a;
  int *b;
  a = 1;
  b = &a;
  return *b;
}
int test2() {
  int a;
  int *b;
  a = 2;
  b = &a;
  return *b;
}
int test3() {
  int x;
  int *y;
  y = &x;
  *y = 3;
  return x;
}
int test4() {
  int *p;
  int *q;
  alloc4(&p, 1, 2, 4, 8);
  q = p + 0;
  return *q;
}
int test5() {
  int *p;
  int *q;
  alloc4(&p, 1, 2, 4, 8);
  q = p + 1;
  return *q;
}
int test6() {
  int *p;
  int *q;
  alloc4(&p, 1, 2, 4, 8);
  q = p + 2;
  return *q;
}
int test7() {
  int *p;
  int *q;
  alloc4(&p, 1, 2, 4, 8);
  q = p + 3;
  return *q;
}
int test8() {
  int *p;
  alloc4(&p, 1, 2, 4, 8);
  p = p + 3;
  p = p - 3;
  return *p;
}
int test9() {
  int *p;
  alloc4(&p, 1, 2, 4, 8);
  p = p + 3;
  p = p - 2;
  return *p;
}
int test10() {
  int *p;
  alloc4(&p, 1, 2, 4, 8);
  p = p + 3;
  p = p - 1;
  return *p;
}
int test11() {
  int *p;
  alloc4(&p, 1, 2, 4, 8);
  p = p + 3;
  p = p - 0;
  return *p;
}
int test12() {
  int *p;
  alloc4(&p, 1, 2, 4, 8);
  return *(p + 0);
}
int test13() {
  int *p;
  alloc4(&p, 1, 2, 4, 8);
  return *(p + 1);
}
int test14() {
  int *p;
  alloc4(&p, 1, 2, 4, 8);
  return *(p + 2);
}
int test15() {
  int *p;
  alloc4(&p, 1, 2, 4, 8);
  return *(p + 3);
}
int test16() {
  int *p;
  alloc4(&p, 1, 2, 4, 8);
  return *((p + 0) + 0);
}
int test17() {
  int *p;
  alloc4(&p, 1, 2, 4, 8);
  return *((p + 1) + 0);
}
int test18() {
  int *p;
  alloc4(&p, 1, 2, 4, 8);
  return *((p + 1) + 1);
}
int test19() {
  int *p;
  alloc4(&p, 1, 2, 4, 8);
  return *((p + 2) + 1);
}
int test20() { return sizeof(int); }
int test21() { return sizeof(int *); }
int test22() {
  int a[16];
  return 0;
}
int test23() {
  int a[16];
  return sizeof(a);
}
int test24() {
  int *a;
  alloc4(&a, 1, 2, 3, 4);
  int *p;
  p = a;
  return *p + *(p + 1);
}
int test25() {
  int a[2];
  *a = 1;
  return *a;
}
int test26() {
  int a[2];
  *(a + 1) = 2;
  return *(a + 1);
}
int test27() {
  int a[2];
  *a = 1;
  *(a + 1) = 2;
  int *p;
  p = a;
  return *p + *(p + 1);
}
int test28() {
  int a[2];
  a[0] = 1;
  a[1] = 2;
  int *p;
  p = a;
  return p[0] + p[1];
}
int test29() {
  int a = 2;
  return *&*&*&*&*&*&*&a;
}
int test30() {
  int a[16];
  int *ptr1 = &a[0];
  int *ptr2 = &a[15];
  return ptr2 - ptr1;
}

int main() {
  test_assert(1, test1(), "int a; int *b; a = 1;b = &a; return *b;");
  test_assert(2, test2(), "int a; int *b; a = 2;b = &a; return *b;");
  test_assert(3, test3(), "int x; int *y; y = &x; *y = 3; return x;");
  test_assert(1, test4(),
         "int *p; int *q; alloc4(&p, 1, 2, 4, 8); q = p + 0; return *q;");
  test_assert(2, test5(),
         "int *p; int *q; alloc4(&p, 1, 2, 4, 8); q = p + 1; return *q;");
  test_assert(4, test6(),
         "int *p; int *q; alloc4(&p, 1, 2, 4, 8); q = p + 2; return *q;");
  test_assert(8, test7(),
         "int *p; int *q; alloc4(&p, 1, 2, 4, 8); q = p + 3; return *q;");
  test_assert(1, test8(),
         "int *p; alloc4(&p, 1, 2, 4, 8); p = p + 3; p = p - 3; return *p;");
  test_assert(2, test9(),
         "int *p; alloc4(&p, 1, 2, 4, 8); p = p + 3; p = p - 2; return *p;");
  test_assert(4, test10(),
         "int *p; alloc4(&p, 1, 2, 4, 8); p = p + 3; p = p - 1; return *p;");
  test_assert(8, test11(),
         "int *p; alloc4(&p, 1, 2, 4, 8); p = p + 3; p = p - 0; return *p;");
  test_assert(1, test12(), "int *p; alloc4(&p, 1, 2, 4, 8); return *(p + 0);");
  test_assert(2, test13(), "int *p; alloc4(&p, 1, 2, 4, 8); return *(p + 1);");
  test_assert(4, test14(), "int *p; alloc4(&p, 1, 2, 4, 8); return *(p + 2);");
  test_assert(8, test15(), "int *p; alloc4(&p, 1, 2, 4, 8); return *(p + 3);");
  test_assert(1, test16(), "int *p; alloc4(&p, 1, 2, 4, 8); return *((p + 0) + 0);");
  test_assert(2, test17(), "int *p; alloc4(&p, 1, 2, 4, 8); return *((p + 1) + 0);");
  test_assert(4, test18(), "int *p; alloc4(&p, 1, 2, 4, 8); return *((p + 1) + 1);");
  test_assert(8, test19(), "int *p; alloc4(&p, 1, 2, 4, 8); return *((p + 2) + 1);");
  test_assert(4, test20(), "return sizeof(int);");
  test_assert(8, test21(), "return sizeof(int*);");
  test_assert(0, test22(), "int a[16]; return 0;");
  test_assert(64, test23(), "int a[16]; return sizeof(a);");
  test_assert(
      3, test24(),
      "int *a; alloc4(&a, 1, 2, 3, 4); int *p; p = a; return *p + *(p + 1);");
  test_assert(1, test25(), "int a[2]; *a = 1; return *a;");
  test_assert(2, test26(), "int a[2]; *(a + 1) = 2; return *(a + 1);");
  test_assert(
      3, test27(),
      "int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; return *p + *(p + 1);");
  test_assert(3, test28(),
         "int a[2]; a[0] = 1; a[1] = 2; int *p; p = a; return p[0] + p[1];");
  test_assert(2, test29(), "int a = 2; return *&*&*&*&*&*&*&a;");
  test_assert(
      15, test30(),
      "int a[16]; int *ptr1 = &a[0]; int *ptr2 = &a[15]; return ptr2 - ptr1;");
  printf("OK\n");
  return 0;
}
