#include "container.h"
#include "tokenize.h"
#include <stdio.h>
#include <stdlib.h>

void assert(int, int, char *);

void assert_string(const String *s1, const String *s2, const char *msg) {
  assert(s1->length, s2->length, "length");
  if (!string_compare(s1, s2)) {
    printf("\033[31m");
    printf("%s => \"%s\"", msg, string_to_char(s2));
    printf(" expected, but got \"%s\"\n", string_to_char(s2));
  } else {
    printf("\033[32m");
    printf("%s => \"%s\"\n", msg, string_to_char(s2));
  }

  printf("\033[39m");
}

void test1() {
  char *input = "void assert(); int main() { int test = 0; assert(); }";
  Token *token = tokenize(input);

  assert(TOKEN_RESERVED, token->kind, "kind");
  assert_string(token->string, char_to_string("void"), "string");
  token = token->next;

  assert(TOKEN_IDENTIFIER, token->kind, "kind");
  assert_string(token->string, char_to_string("assert"), "string");
  token = token->next;

  assert(TOKEN_RESERVED, token->kind, "kind");
  assert_string(token->string, char_to_string("("), "string");
  token = token->next;

  assert(TOKEN_RESERVED, token->kind, "kind");
  assert_string(token->string, char_to_string(")"), "string");
  token = token->next;

  assert(TOKEN_RESERVED, token->kind, "kind");
  assert_string(token->string, char_to_string(";"), "string");
  token = token->next;

  assert(TOKEN_RESERVED, token->kind, "kind");
  assert_string(token->string, char_to_string("int"), "string");
  token = token->next;

  assert(TOKEN_IDENTIFIER, token->kind, "kind");
  assert_string(token->string, char_to_string("main"), "string");
  token = token->next;

  assert(TOKEN_RESERVED, token->kind, "kind");
  assert_string(token->string, char_to_string("("), "string");
  token = token->next;

  assert(TOKEN_RESERVED, token->kind, "kind");
  assert_string(token->string, char_to_string(")"), "string");
  token = token->next;

  assert(TOKEN_RESERVED, token->kind, "kind");
  assert_string(token->string, char_to_string("{"), "string");
  token = token->next;

  assert(TOKEN_RESERVED, token->kind, "kind");
  assert_string(token->string, char_to_string("int"), "string");
  token = token->next;

  assert(TOKEN_IDENTIFIER, token->kind, "kind");
  assert_string(token->string, char_to_string("test"), "string");
  token = token->next;

  assert(TOKEN_RESERVED, token->kind, "kind");
  assert_string(token->string, char_to_string("="), "string");
  token = token->next;

  assert(TOKEN_NUMBER, token->kind, "kind");
  assert_string(token->string, char_to_string("0"), "string");
  token = token->next;

  assert(TOKEN_RESERVED, token->kind, "kind");
  assert_string(token->string, char_to_string(";"), "string");
  token = token->next;

  assert(TOKEN_IDENTIFIER, token->kind, "kind");
  assert_string(token->string, char_to_string("assert"), "string");
  token = token->next;

  assert(TOKEN_RESERVED, token->kind, "kind");
  assert_string(token->string, char_to_string("("), "string");
  token = token->next;

  assert(TOKEN_RESERVED, token->kind, "kind");
  assert_string(token->string, char_to_string(")"), "string");
  token = token->next;

  assert(TOKEN_RESERVED, token->kind, "kind");
  assert_string(token->string, char_to_string(";"), "string");
  token = token->next;

  assert(TOKEN_RESERVED, token->kind, "kind");
  assert_string(token->string, char_to_string("}"), "string");
  token = token->next;
}

int main() {
  test1();
  return 0;
}
