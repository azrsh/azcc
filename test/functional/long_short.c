#include "testtool.h"

int main() {
  {
    int short i = 1 << 15;
    test_assert(1, i < 0, "1 << 15 < 0");
  }
  {
    short int i = 1 << 15;
    test_assert(1, i < 0, "1 << 15 < 0");
  }
  {
    short i = 1 << 15;
    test_assert(1, i < 0, "1 << 15 < 0");
  }

  {
    int long i = 1;
    i = i << 63;
    test_assert(1, i < 0, "1 << 63 < 0");
  }
  {
    long int i = 1;
    i = i << 63;
    test_assert(1, i < 0, "1 << 63 < 0");
  }
  {
    long i = 1;
    i = i << 63;
    test_assert(1, i < 0, "1 << 63 < 0");
  }
  {
    long long i = 1;
    i = i << 63;
    test_assert(1, i < 0, "1 << 63 < 0");
  }
  return 0;
}
