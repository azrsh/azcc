
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

int test7() {
  int b;
  if (1)
    return 5 * (9 - 6);
  b = 26 - 3 + 2;
  return b;
}

int test8() {
  int b;
  if (0)
    return 5 * (9 - 6);
  b = 26 - 3 + 2;
  return 10;
}

int test9() {
  int b;
  if (0)
    return 0;
  else
    return 5 * (9 - 6);
  b = 26 - 3 + 2;
  return b;
}

int test10() {
  while (0)
    return 0;
  return 15;
}

int test11() {
  int a;
  a = 10;
  while (a > 0)
    a = a - 1;
  return 15;
}

int test12() {
  int a;
  int b;
  b = 0;
  for (a = 0; a < 10; a = a + 1)
    b = b + 1;
  return b;
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
  return c;
}

int main() {
  assert(16, test1(), "int a; int b; a = 5*(9-6);b=a+1;");
  assert(44, test2(), "int a; int b; a=29;b=5*(9-6);a+b;");
  assert(25, test3(), "int a; int b; a=5*(9-6);b=26-3+2;");
  assert(44, test4(), "int asaskjkas; int bakjehjioaj; ..."
         /*"asaskjkas=29;bakjehjioaj=5*(9-6);asaskjkas+bakjehjioa"*/);
  assert(40, test5(), "int a; int b; a=5*(9-6);b=26-3+2;return a + b;");
  assert(15, test6(), "int b; return 5*(9-6);b=26-3+2;");
  assert(15, test7(), "int b; if(1)return 5*(9-6);b=26-3+2;");
  assert(10, test8(), "int b; if(0)return 5*(9-6);b=26-3+2;return 10;");
  assert(15, test9(), "int b; if(0)return 0; else return 5*(9-6);b=26-3+2;");
  assert(15, test10(), "while(0)return 0; return 15;");
  assert(15, test11(), "int a; a = 10;while(a > 0)a = a - 1; return 15;");
  assert(10, test12(),
         "int a; int b; b = 0;for(a = 0;a < 10;a = a + 1)b = b + 1; return b;");
  assert(
      20, test13(),
      "int a; int b; int c; b = 0;c = 0;for(a = 0;a < 10;a = a + 1){b = b ..."
      /*"+ 1;c = c + 2;} return c;}"*/);
  printf("OK\n");
  return 0;
}
