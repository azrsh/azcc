int test_assert(int, int, char *);
int printf();

void test1() {
  int a;
  int b;
  a = 5 * (9 - 6);
  b = a + 1;
  test_assert(16, b, "int a; int b; a = 5*(9-6);b=a+1;");
}

void test2() {
  int a;
  int b;
  a = 29;
  b = 5 * (9 - 6);
  test_assert(44, a + b, "int a; int b; a=29;b=5*(9-6);a+b;");
}

void test3() {
  int a;
  int b;
  a = 5 * (9 - 6);
  b = 26 - 3 + 2;
  test_assert(25, b, "int a; int b; a=5*(9-6);b=26-3+2;");
}

void test4() {
  int asaskjkas;
  int bakjehjioaj;
  asaskjkas = 29;
  bakjehjioaj = 5 * (9 - 6);
  test_assert(44, asaskjkas + bakjehjioaj, "int asaskjkas; int bakjehjioaj; ..."
              /*"asaskjkas=29;bakjehjioaj=5*(9-6);asaskjkas+bakjehjioa"*/);
}

void test5() {
  int a;
  int b;
  a = 5 * (9 - 6);
  b = 26 - 3 + 2;
  test_assert(40, a + b, "int a; int b; a=5*(9-6);b=26-3+2;return a + b;");
}

void test6() {
  int b;
  test_assert(15, 5 * (9 - 6), "int b; return 5*(9-6);b=26-3+2;");
  b = 26 - 3 + 2;
}

void test7() {
  int a = 1;
  test_assert(1, a, "int a = 1;");
  int b = a;
  test_assert(1, b, "int b = a;");
  int c = a + b;
  test_assert(2, c, "int c = a + b;");
  int d = a * b + c;
  test_assert(3, d, "int d = a * b + c;");
}

int test8() {
  int a;
  int b;
  a = 5 * (9 - 6);
  b = a + 1;
  return b;
}

int test9() {
  int a;
  int b;
  a = 29;
  b = 5 * (9 - 6);
  return a + b;
}

int test10() {
  int a;
  int b;
  a = 5 * (9 - 6);
  b = 26 - 3 + 2;
  return b;
}

int test11() {
  int asaskjkas;
  int bakjehjioaj;
  asaskjkas = 29;
  bakjehjioaj = 5 * (9 - 6);
  return asaskjkas + bakjehjioaj;
}

int test12() {
  int a;
  int b;
  a = 5 * (9 - 6);
  b = 26 - 3 + 2;
  return a + b;
}

int test13() {
  int b;
  return 5 * (9 - 6);
  b = 26 - 3 + 2;
}

int main() {
  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  test7();

  test_assert(16, test8(), "int a; int b; a = 5*(9-6);b=a+1;");
  test_assert(44, test9(), "int a; int b; a=29;b=5*(9-6);a+b;");
  test_assert(25, test10(), "int a; int b; a=5*(9-6);b=26-3+2;");
  test_assert(44, test11(), "int asaskjkas; int bakjehjioaj; ..."
              /*"asaskjkas=29;bakjehjioaj=5*(9-6);asaskjkas+bakjehjioa"*/);
  test_assert(40, test12(), "int a; int b; a=5*(9-6);b=26-3+2;return a + b;");
  test_assert(15, test13(), "int b; return 5*(9-6);b=26-3+2;");
  printf("OK\n");
  return 0;
}
