int assert(int, int, char *);
int printf();

int test1() { return 'a'; }
int test2() { return 'b'; }
int test3() { return 'c'; }
int test4() { return 'd'; }
int test5() { return 'e'; }
int test6() { return 'f'; }

int main() {
  assert(97, test1(), "\'a\'");
  assert(98, test2(), "\'b\'");
  assert(99, test3(), "\'c\'");
  assert(100, test4(), "\'d\'");
  assert(101, test5(), "\'e\'");
  assert(102, test6(), "\'f\'");
  return 0;
}
