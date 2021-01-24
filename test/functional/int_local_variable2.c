int test_assert(int, int, char *);
int printf();

int test1() {
  int a;
  int b;
  a = 5 * (9 - 6);
  b = a + 1;
  return b;
}

int test2() {
  int a;
  int b;
  a = 29;
  b = 5 * (9 - 6);
  return a + b;
}

int test3() {
  int a;
  int b;
  a = 5 * (9 - 6);
  b = 26 - 3 + 2;
  return b;
}

int test4() {
  int asaskjkas;
  int bakjehjioaj;
  asaskjkas = 29;
  bakjehjioaj = 5 * (9 - 6);
  return asaskjkas + bakjehjioaj;
}

int test5() {
  int a;
  int b;
  a = 5 * (9 - 6);
  b = 26 - 3 + 2;
  return a + b;
}

int test6() {
  int b;
  return 5 * (9 - 6);
  b = 26 - 3 + 2;
}

int main() {
  test_assert(16, test1(), "int a; int b; a = 5*(9-6);b=a+1;");
  test_assert(44, test2(), "int a; int b; a=29;b=5*(9-6);a+b;");
  test_assert(25, test3(), "int a; int b; a=5*(9-6);b=26-3+2;");
  test_assert(44, test4(), "int asaskjkas; int bakjehjioaj; ..."
         /*"asaskjkas=29;bakjehjioaj=5*(9-6);asaskjkas+bakjehjioa"*/);
  test_assert(40, test5(), "int a; int b; a=5*(9-6);b=26-3+2;return a + b;");
  test_assert(15, test6(), "int b; return 5*(9-6);b=26-3+2;");
  printf("OK\n");
  return 0;
}
