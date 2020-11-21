int test1sub() {
  int b;
  if (1)
    return 5 * (9 - 6);
  b = 26 - 3 + 2;
  return b;
}
int test1() {
  assert(15, test1sub(), "int b; if(1)return 5*(9-6);b=26-3+2;");
  return 0;
}

int test2sub() {
  int b;
  if (0)
    return 5 * (9 - 6);
  b = 26 - 3 + 2;
  return 10;
}
int test2() {
  assert(10, test2sub(), "int b; if(0)return 5*(9-6);b=26-3+2;return 10;");
  return 0;
}

int test3sub() {
  int b;
  if (0)
    return 0;
  else
    return 5 * (9 - 6);
  b = 26 - 3 + 2;
  return b;
}
int test3() {
  assert(15, test3sub(), "int b; if(0)return 0; else return 5*(9-6);b=26-3+2;");
  return 0;
}

int test4sub() {
  while (0)
    return 0;
  return 15;
}
int test4() {
  int x;
  x = test4sub();
  assert(15, x, "while(0)return 0; return 15;");
  return 0;
}

int test5sub() {
  int a;
  a = 10;
  while (a > 0)
    a = a - 1;
  return 15;
}
int test5() {
  assert(15, test5sub(), "int a; a = 10;while(a > 0)a = a - 1; return 15;");
  return 0;
}

int test6sub() {
  int a;
  int b;
  b = 0;
  for (a = 0; a < 10; a = a + 1)
    b = b + 1;
  return b;
}
int test6() {
  assert(10, test6sub(),
         "int a; int b; b = 0;for(a = 0;a < 10;a = a + 1)b = b + 1; return b;");
  return 0;
}

int test7() {
  int a;
  int b;
  int c;
  b = 0;
  c = 0;
  for (a = 0; a < 10; a = a + 1) {
    b = b + 1;
    c = c + 2;
  }
  assert(
      20, c,
      "int a; int b; int c; b = 0;c = 0;for(a = 0;a < 10;a = a + 1){b = b ..."
      /*"+ 1;c = c + 2;} return c;}"*/);
  return 0;
}

int test8sub() {
  int a;
  int b;
  int c;
  b = 0;
  c = 0;
  for (a = 0; a < 10; a = a + 1) {
    b = b + 1;
    c = c + 2;
  }
  return c;
}
int test8() {
  assert(
      20, test8sub(),
      "int a; int b; int c; b = 0;c = 0;for(a = 0;a < 10;a = a + 1){b = b ..."
      /*"+ 1;c = c + 2;} return c;}"*/);
  return 0;
}

int test9sub() {
  int x = 128;
  for (int i = 0; i < 10; i = i + 1) {
    x = x + 2;
  }
  return x;
}
int test9() {
  assert(148, test9sub(),
         "int x = 128; for (int i = 0; i < 10; i = i + 1) { x = x + 2; }..."
         /*"return x;"*/);
  return 0;
}

int test10sub() {
  int x = 0;
  for (int i = 0; i < 10; i = i + 1) {
    for (int j = 0; j < 10; j = j + 1) {
      x = x + i * j;
    }
  }
  return x;
}
int test10() {
  assert(
      2025, test10sub(),
      "int x = 0; for (int i = 0; i < 10; i = i + 1) { for (int j = 0; j <..."
      /*"10; j = j + 1) { x = x + i * j; } }return x;"*/);
  return 0;
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
