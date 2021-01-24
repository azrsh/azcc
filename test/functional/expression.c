int test_assert(int, int, char *);
int printf();

int main() {
  test_assert(0, 0, "0");
  test_assert(42, 42, "42");
  test_assert(21, 5 + 20 - 4, "5 + 20 - 4");
  test_assert(41, 12 + 34 - 5, "12 + 34 - 5");
  test_assert(47, 5 + 6 * 7, "5 + 6 * 7");
  test_assert(15, 5 * (9 - 6), "5 * (9 - 6)");
  test_assert(4, (3 + 5) / 2, "(3 + 5) / 2");
  test_assert(10, -10 + 20, "-10 + 20");
  test_assert(2, -10 / -5, "-10 / -5");
  test_assert(1, 10 == 10, "10 - 10");
  test_assert(0, 10 < 10, "10 < 10");
  test_assert(1, 20 - 10 * 1 + 2 <= 2 * 7, "20 - 10 * 1 + 2 <= 2 * 7");
  test_assert(0, 80 - 50 > 20 * 2, "80 - 50 > 20 * 2");
  test_assert(1, 80 - 50 < 20 * 2, "80 - 50 < 20 * 2");
  test_assert(0, 80 - 0 <= 20 * 2, "80 - 0 <= 20 * 2");
  test_assert(1, 0 <= 16, "0 <= 16");
  test_assert(0, 0 >= 16, "0 >= 16");
  test_assert(0, 80 % 4, "80 % 4");
  test_assert(3, 79 % 4, "79 % 4");
  test_assert(28, 1 ? 28 : 25, "1 ? 28 : 25");
  test_assert(28, 1241 ? 28 : 25, "1 ? 28 : 25");
  test_assert(25, 0 ? 28 : 25, "1 ? 28 : 25");

  test_assert(4, sizeof(int), "sizeof(int)");
  test_assert(8, sizeof(int *), "sizeof(int*)");
  test_assert(1, sizeof(char), "sizeof(char)");
  test_assert(8, sizeof(char *), "sizeof(char*)");

  test_assert(4, _Alignof(int), "_Alignof(int)");
  test_assert(8, _Alignof(int *), "_Alignof(int*)");
  test_assert(1, _Alignof(char), "_Alignof(char)");
  test_assert(8, _Alignof(char *), "_Alignof(char*)");

  printf("OK\n");
  return 0;
}
