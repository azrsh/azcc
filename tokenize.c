#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"

void error_at(char *location, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int position = location - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s\n", position, "");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(EXIT_FAILURE);
}

// printfと同じ引数をとる
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(EXIT_FAILURE);
}

bool at_eof() { return token->kind == TOKEN_EOF; }

//新しいトークンを作成してcurrentに繋げる
Token *new_token(TokenKind kind, Token *current, char *string, int length) {
  Token *token = calloc(1, sizeof(Token));
  token->kind = kind;
  token->string = string;
  token->length = length;
  current->next = token;
  return token;
}

bool start_with(char *p, char *q) { return memcmp(p, q, strlen(q)) == 0; }

Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *current = &head;

  while (*p) {
    //空白文字をスキップ
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (start_with(p, ">=") || start_with(p, "<=") || start_with(p, "==") ||
        start_with(p, "!=")) {
      current = new_token(TOKEN_RESERVED, current, p, 2);
      p += 2;
      continue;
    }

    if (strchr("+-*/()<>", *p)) {
      current = new_token(TOKEN_RESERVED, current, p++, 1);
      continue;
    }

    if (isdigit(*p)) {
      current = new_token(TOKEN_NUMBER, current, p, 0);
      char *q = p;
      current->value = strtol(p, &p, 10);
      current->length = p - q;
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  new_token(TOKEN_EOF, current, p, 0);
  return head.next;
}

