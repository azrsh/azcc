#include "argument.h"
#include "codegen.h"
#include "config.h"
#include "parse.h"
#include "tokenize.h"
#include "util.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  Config *config = parse_argument(argc, argv);

  if (config->filename) {
    filename = config->filename;
    user_input = read_path(filename);
  } else {
    filename = config->filename = "none";
    user_input = read_stdin();
  }

  Token *head = tokenize(user_input);
  AbstractSyntaxTree *ast = parse(head);
  generate_code(ast, config);

  return EXIT_SUCCESS;
}
