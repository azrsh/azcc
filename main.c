#include "codegen.h"
#include "parse.h"
#include "tokenize.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return EXIT_FAILURE;
  }

  filename = argv[1];
  user_input = read_file(filename);

  //トークナイズしてパースする
  Token *head = tokenize(user_input);
  Program *program = parse(head); // FunctionDefinition List

  //コード生成
  generate_code(program);

  return EXIT_SUCCESS;
}
