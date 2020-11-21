int test1() {
  int a;
  int b;
  a = 5 * (9 - 6);
  b = a + 1;
  assert(16, b, "int a; int b; a = 5*(9-6);b=a+1;");
  return 0;
}

int test2() {
  int a;
  int b;
  a = 29;
  b = 5 * (9 - 6);
  assert(44, a + b, "int a; int b; a=29;b=5*(9-6);a+b;");
  return 0;
}

int test3() {
  int a;
  int b;
  a = 5 * (9 - 6);
  b = 26 - 3 + 2;
  assert(25, b, "int a; int b; a=5*(9-6);b=26-3+2;");
  return 0;
}

int test4() {
  int asaskjkas;
  int bakjehjioaj;
  asaskjkas = 29;
  bakjehjioaj = 5 * (9 - 6);
  assert(44, asaskjkas + bakjehjioaj, "int asaskjkas; int bakjehjioaj; ..."
         /*"asaskjkas=29;bakjehjioaj=5*(9-6);asaskjkas+bakjehjioa"*/);
  return 0;
}

int test5() {
  int a;
  int b;
  a = 5 * (9 - 6);
  b = 26 - 3 + 2;
  assert(40, a + b, "int a; int b; a=5*(9-6);b=26-3+2;return a + b;");
  return 0;
}

int test6() {
  int b;
  assert(15, 5 * (9 - 6), "int b; return 5*(9-6);b=26-3+2;");
  b = 26 - 3 + 2;
  return 0;
}

int test7sub() {
  int b;
  if (1)
    return 5 * (9 - 6);
  b = 26 - 3 + 2;
  return b;
}
int test7() {
  assert(15, test7sub(), "int b; if(1)return 5*(9-6);b=26-3+2;");
  return 0;
}

int test8sub() {
  int b;
  if (0)
    return 5 * (9 - 6);
  b = 26 - 3 + 2;
  return 10;
}
int test8() {
  assert(10, test8sub(), "int b; if(0)return 5*(9-6);b=26-3+2;return 10;");
  return 0;
}

int test9sub() {
  int b;
  if (0)
    return 0;
  else
    return 5 * (9 - 6);
  b = 26 - 3 + 2;
  return b;
}
int test9() {
  assert(15, test9sub(), "int b; if(0)return 0; else return 5*(9-6);b=26-3+2;");
  return 0;
}

int test10sub() {
  while (0)
    return 0;
  return 15;
}
int test10() {
  int x;
  x = test10sub();
  assert(15, x, "while(0)return 0; return 15;");
  return 0;
}

int test11sub() {
  int a;
  a = 10;
  while (a > 0)
    a = a - 1;
  return 15;
}
int test11() {
  assert(15, test11sub(), "int a; a = 10;while(a > 0)a = a - 1; return 15;");
  return 0;
}

int test12sub() {
  int a;
  int b;
  b = 0;
  for (a = 0; a < 10; a = a + 1)
    b = b + 1;
  return b;
}
int test12() {
  assert(10, test12sub(),
         "int a; int b; b = 0;for(a = 0;a < 10;a = a + 1)b = b + 1; return b;");
  return 0;
}

int test13() {
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

int test14() {
  int a = 1;
  assert(1, a, "int a = 1;");
  int b = a;
  assert(1, b, "int b = a;");
  int c = a + b;
  assert(2, c, "int c = a + b;");
  int d = a * b + c;
  assert(3, d, "int d = a * b + c;");
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
  test11();
  test12();
  test13();
  test14();
  printf("OK\n");
  return 0;
}
