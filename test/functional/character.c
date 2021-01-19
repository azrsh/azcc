int test_assert(int, int, char *);
int printf();

int test1() { return 'a'; }
int test2() { return 'b'; }
int test3() { return 'c'; }
int test4() { return 'd'; }
int test5() { return 'e'; }
int test6() { return 'f'; }

int main() {
  test_assert(97, test1(), "\'a\'");
  test_assert(98, test2(), "\'b\'");
  test_assert(99, test3(), "\'c\'");
  test_assert(100, test4(), "\'d\'");
  test_assert(101, test5(), "\'e\'");
  test_assert(102, test6(), "\'f\'");
  return 0;
}
