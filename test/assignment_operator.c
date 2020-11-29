int assert(int, int, char *);
int printf();

int test1() {
  int a = 5;
  a *= (9 - 6);
  a += 1;
  assert(16, a, "int a = 5*(9-6); a += 1;");
  return 0;
}

int test2() {
  int a = 29;
  a += 5 * (9 - 6);
  assert(44, a, "int a = 29; a += 5*(9-6);");
  return 0;
}

int test3() {
  int a = 26;
  a -= 3 - 2;
  assert(25, a, "int a; int b; a=5*(9-6);b=26-3+2;");
  return 0;
}

int test4() {
  int asaskjkas = 29;
  asaskjkas += 5 * (9 - 6);
  assert(44, asaskjkas, "int asaskjkas = 29; asaskjkas += 5*(9-6);");
  return 0;
}

int test5() {
  int a = 5 * (9 - 6);
  a += 26 - 3 + 2;
  assert(40, a, "int a; int b; a=5*(9-6);b=26-3+2;return a + b;");
  return 0;
}

int main() {
  test1();
  test2();
  test3();
  test4();
  test5();
  printf("OK\n");
  return 0;
}
