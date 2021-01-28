#include "container.h"
#include "parse.h"
#include "tokenize.h"
#include <stdio.h>
#include <stdlib.h>

void test_assert(int, int, char *);

int main() {
  user_input =
      "_Bool test6(int x) { if (x == 10) { return x; } return x == 0; }";

  Token *head = tokenize(user_input);
  parse(head);
  return 0;
}
