int test_assert(int, int, char *);
int printf();

int foo();
int foo1(int);
int foo2(int, int);
int test1(int);
int test2(int, int);
int test3(int, int, int);
int test4(int, int, int, int);
int test5(int, int, int, int, int);

int test1(int a) { return a; }
int test2(int a, int b) { return a + b; }
int test3(int a, int b, int c) { return a + b + c; }
int test4(int a, int b, int c, int d) { return a + b + c + d; }
int test5(int a, int b, int c, int d, int e) { return a + b + c + d + e; }

int main() {
  test_assert(0, foo(), "foo()");
  test_assert(13, foo1(13), "foo1(13)");
  test_assert(25, foo2(13, 12), "foo2(13,12)");
  test_assert(1, test1(1), "test1(1)");
  test_assert(3, test2(1, 2), "test2(1, 2)");
  test_assert(6, test3(1, 2, 3), "test3(1, 2, 3)");
  test_assert(10, test4(1, 2, 3, 4), "test4(1, 2, 3, 4)");
  test_assert(15, test5(1, 2, 3, 4, 5), "test5(1, 2, 3, 4, 5)");
  test_assert(6, test1(1) + test1(2) + test1(3), "test1(1) + test1(2) + test1(3)");
  printf("OK\n");
  return 0;
}
