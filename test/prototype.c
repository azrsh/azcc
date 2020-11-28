int foo();
int foo1(int a);
int foo2(int a, int b);

int test1(int a) { return a; }
int test2(int a, int b) { return a + b; }
int test3(int a, int b, int c) { return a + b + c; }

int main() {
  assert(0, foo(), "foo()");
  assert(13, foo1(13), "foo1(13)");
  assert(25, foo2(13, 12), "foo2(13,12)");
  assert(1, test1(1), "test1(1)");
  assert(3, test2(1, 2), "test2(1, 2)");
  assert(6, test3(1, 2, 3), "test3(1, 2, 3)");
  assert(6, test1(1) + test1(2) + test1(3), "test1(1) + test1(2) + test1(3)");
  printf("OK\n");
  return 0;
}
