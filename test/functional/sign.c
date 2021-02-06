#include "testtool.h"

int main() {
  {
    signed int i1 = 1 << 31;
    test_assert(1, i1 < 0, "1 << 31 < 0");
    unsigned int i2 = 1 << 31;
    test_assert(1, i2 > 0, "1 << 31 > 0");
    int i3 = 1 << 31;
    test_assert(1, i3 < 0, "1 << 31 < 0");
    signed i4 = 1 << 31;
    test_assert(1, i4 < 0, "1 << 31 < 0");
    unsigned i5 = 1 << 31;
    test_assert(1, i5 > 0, "1 << 31 > 0");
  }
  {
    signed int i1 = 1 << 31;
    test_assert(1, 0 > i1, "0 > 1 << 31");
    unsigned int i2 = 1 << 31;
    test_assert(1, 0 < i2, "0 < 1 << 31");
    int i3 = 1 << 31;
    test_assert(1, 0 > i3, "0 > 1 << 31");
    signed i4 = 1 << 31;
    test_assert(1, 0 > i4, "0 > 1 << 31");
    unsigned i5 = 1 << 31;
    test_assert(1, 0 < i5, "0 < 1 << 31");
  }
  return 0;
}
