#include "9cc.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return EXIT_FAILURE;
  }

  //トークナイズしてパースする
  user_input = argv[1];
  token = tokenize(user_input);
  Node *node = expression();

  //コード生成
  generate_code(node);

  return EXIT_SUCCESS;
}
