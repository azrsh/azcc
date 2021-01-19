#include "testtool.h"
#include <stdbool.h>
#include <stdio.h>

int main() {
  test_assert(1, 1 && 1, "1 && 1");
  test_assert(0, 1 && 0, "1 && 0");
  test_assert(0, 0 && 1, "0 && 1");
  test_assert(0, 0 && 0, "0 && 0");
  test_assert(1, 1 || 1, "1 || 1");
  test_assert(1, 1 || 0, "1 || 0");
  test_assert(1, 0 || 1, "0 || 1");
  test_assert(0, 0 || 0, "0 || 0");

  test_assert(0, !(10 == 10), "!(10 == 10)");
  test_assert(1, !(10 < 10), "!(10 < 10)");
  test_assert(0, !(20 - 10 * 1 + 2 <= 2 * 7), "!(20 - 10 * 1 + 2 <= 2 * 7)");
  test_assert(1, !(80 - 50 > 20 * 2), "!(80 - 50 > 20 * 2)");
  test_assert(0, !(80 - 50 < 20 * 2), "!(80 - 50 < 20 * 2)");
  test_assert(1, !(80 - 0 <= 20 * 2), "!(80 - 0 <= 20 * 2)");

  test_assert(0, !(10 == 10) && 10 == 10, "!(10 == 10) && 10 == 10");
  test_assert(1, !(10 == 10) || 10 == 10, "!(10 == 10) || 10 == 10");
  test_assert(0, !(1), "!(1)");
  test_assert(0, !(1) || 10 == 10 && !(10 == 10), "!(1) || 10 == 10 && !(10 == 10)");

  test_assert(1, 2 && 1, "2 && 1");
  test_assert(1, 1 && 2 < 174, "1 && 2 < 174");
  test_assert(1, 2 && 2 < 174, "2 && 2 < 174");
  bool res = true;
  for (int a = 1; a < 174; a++)
    res = res && a && a < 174;
  test_assert(1, res, "a && a < 174");

  test_assert(174, (3 && 2 && 5) + 173, "(3 && 2 && 5) + 173");

  printf("OK\n");
  return 0;
}
