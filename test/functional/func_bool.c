#include "testtool.h"
#include <stdbool.h>
#include <stdio.h>

bool test1() { return 0; }
bool test2() { return 1; }
bool test3() { return 13953; }
bool test4() { return true; }
bool test5() { return false; }
bool test6(int x) {
  if (x == 10) {
    return x;
  }
  return x == 0;
}

int main() {
  assert(0, test1(), "{ return 0; }");
  assert(1, test2(), "{ return 1; }");
  assert(1, test3(), "{ return 13953; }");
  assert(1, test4(), "{ return true; }");
  assert(0, test5(), "{ return false; }");
  assert(0, test6(1), "{ if(x == 10) { return x; } return x == 0; }");
  assert(1, test6(0), "{ if(x == 10) { return x; } return x == 0; }");
  assert(0, test6(1324), "{ if(x == 10) { return x; } return x == 0; }");
  assert(1, test6(10), "{ if(x == 10) { return x; } return x == 0; }");
  printf("OK\n");
  return 0;
}
