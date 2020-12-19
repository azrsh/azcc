#include "codegen.h"
#include "parse.h"
#include "tokenize.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  if (argc == 1) {
    filename = "none";
    user_input = read_stdin();
  } else if (argc == 2) {
    filename = argv[1];
    user_input = read_path(filename);
  } else {
    ERROR("引数の個数が正しくありません\n");
    return EXIT_FAILURE;
  }

  //トークナイズしてパースする
  Token *head = tokenize(user_input);
  Program *program = parse(head); // FunctionDefinition List

  //コード生成
  generate_code(program);

  return EXIT_SUCCESS;
}
