int test1(int a) { return a; }
int test2(int a, int b) { return a + b; }
int test3(int a, int b, int c) { return a + b + c; }
int test4(int a, int b, int c, int d) { return a + b + c + d; }
int test5(int a, int b, int c, int d, int e) { return a + b + c + d + e; }
int test6(int a, int b, int c, int d, int e, int f) {
  return a + b + c + d + e + f;
}
int test7(int a, int b, int c, int d, int e, int f, int g) {
  return a + b + c + d + e + f + g;
}
int test8(int a, int b, int c, int d, int e, int f, int g, int h) {
  return a + b + c + d + e + f + g + h;
}
int test9(int a, int b, int c, int d, int e, int f, int g, int h, int i) {
  return a + b + c + d + e + f + g + h + i;
}
int test10(int a, int b, int c, int d, int e, int f, int g, int h, int i,
           int j) {
  return a + b + c + d + e + f + g + h + i + j;
}

int main() {
  /*assert(0, foo(), "foo()");
  assert(13, foo1(13), "foo1(13)");
  assert(25, foo2(13, 12), "foo2(13,12)");
  assert(36, foo3(13, 12, 11), "foo3(13,12,11)");
  assert(70, foo7(13, 12, 11, 10, 9, 8, 7), "foo7(13,12,11,10,9,8,7)");
  assert(76, foo8(13, 12, 11, 10, 9, 8, 7, 6), "foo8(13,12,11,10,9,8,7,6)");
  assert(81, foo9(13, 12, 11, 10, 9, 8, 7, 6, 5),
         "foo9(13,12,11,10,9,8,7,6,5)");
  assert(85, foo10(13, 12, 11, 10, 9, 8, 7, 6, 5, 4),
         "foo10(13,12,11,10,9,8,7,6,5,4)");*/
  /*assert(1, test1(1), "test1(1)");
  assert(3, test2(1, 2), "test2(1, 2)");
  assert(6, test3(1, 2, 3), "test3(1, 2, 3)");
  assert(10, test4(1, 2, 3, 4), "test4(1, 2, 3, 4)");
  assert(15, test5(1, 2, 3, 4, 5), "test5(1, 2, 3, 4, 5)");
  assert(21, test6(1, 2, 3, 4, 5, 6), "test6(1, 2, 3, 4, 5, 6)");
  assert(28, test7(1, 2, 3, 4, 5, 6, 7), "test7(1, 2, 3, 4, 5, 6, 7)");
  assert(36, test8(1, 2, 3, 4, 5, 6, 7, 8), "test8(1, 2, 3, 4, 5, 6, 7, 8)");
  assert(45, test9(1, 2, 3, 4, 5, 6, 7, 8, 9),
         "test9(1, 2, 3, 4, 5, 6, 7, 8, 9)");
  assert(55, test10(1, 2, 3, 4, 5, 6, 7, 8, 9, 10),
         "test10(1, 2, 3, 4, 5, 6, 7, 8, 9, 10)");
  assert(6, test1(1) + test1(2) + test1(3), "test1(1) + test1(2) + test1(3)");*/
  /* assert(110, test10(1, 2, 3, 4, 5, 6, 7, 8, 9, 10) + test10(1, 2, 3, 4, 5,
   * 6, 7, 8, 9, 10), "test10(1, 2, 3, 4, 5, 6, 7, 8, 9, 10) + test10(1, 2, 3,
   * 4, 5,* 6, 7, 8, 9, 10)"); */
  printf("OK\n");
  return 0;
}
