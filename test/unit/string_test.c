#include "container.h"
#include <stdio.h>
#include <stdlib.h>

void assert(int, int, char *);
void assert_string(const String *s1, const String *s2, const char *msg) {
  if (!string_compare(s1, s2)) {
    printf("\033[31m");
    printf("%s => \"%s\"", msg, string_to_char(s2));
    printf(" expected, but got \"%s\"\n", string_to_char(s2));
    printf("\033[39m");
    return;
  }

  printf("\033[32m");
  printf("%s => \"%s\"\n", msg, string_to_char(s2));
  printf("\033[39m");
}

void test1(const char *target) {
  const String *s = char_to_string(target);
  assert_string(s, s, "test no cp");
}
void test2(const char *target) {
  const String *s1 = char_to_string(target);
  const String *s2 = char_to_string(target);
  assert_string(s1, s2, "test cp");
}

int main() {
  test1("void");
  test1("int");
  test1("x");
  test1("3");

  test2("void");
  test2("int");
  test2("x");
  test2("3");
  return 0;
}
