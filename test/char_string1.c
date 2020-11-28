int assert(int, int, char *);
int printf();

int test1() {
  int x[3];
  x[0] = -1;
  x[1] = 2;
  int y;
  y = 4;
  assert(3, x[0] + y,
         "int x[3]; x[0] = -1; x[1] = 2; int y; y = 4; return x[0] + y;");
  return 0;
}
int test2() {
  char x[3];
  x[0] = 1;
  assert(1, x[0], "char x[3]; x[0] = 1; return x[0];");
  return 0;
}
int test3() {
  char x[3];
  x[0] = 1;
  x[1] = 2;
  int y;
  y = x[0];
  assert(1, y, "char x[3]; x[0] = 1; x[1] = 2; int y; y = x[0]; return y;");
  return 0;
}
int test4() {
  char x[3];
  x[0] = 1;
  x[1] = 2;
  int y;
  y = 4;
  assert(5, x[0] + y,
         "char x[3]; x[0] = 1; x[1] = 2; int y; y = 4; return x[0] + y;");
  return 0;
}
int test5() {
  char x[3];
  x[0] = -1;
  x[1] = 2;
  int y;
  y = 4;
  assert(3, x[0] + y,
         "char x[3]; x[0] = -1; x[1] = 2; int y; y = 4; return x[0] + y; ");
  return 0;
}
int test6() {
  char *x;
  x = "abcdefghij";
  assert(97, x[0], "char* x; x = abcdefghij; return x[0];");
  return 0;
}
int test7() {
  char *x;
  x = "abcdefghij";
  assert(98, x[1], "char* x; x = abcdefghij; return x[1];");
  return 0;
}
int test8() {
  char *x;
  x = "abcdefghij";
  assert(99, x[2], "char* x; x = abcdefghij; return x[2];");
  return 0;
}
int test9() {
  char *x;
  x = "abcdefghij";
  assert(100, x[3], "char* x; x = abcdefghij; return x[3];");
  return 0;
}
int test10() {
  char *x;
  x = "abcdefghij";
  assert(101, x[4], "char* x; x = abcdefghij; return x[4];");
  return 0;
}
int test11() {
  char *x;
  x = "abcdefghij";
  assert(102, x[5], "char* x; x = abcdefghij; return x[5];");
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
  printf("OK\n");
  return 0;
}
