#include "9cc.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *user_input;

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
bool is_identifier_initial(char p) { return isalpha(p) || p == '_'; }
bool is_identifier(char p) { return is_identifier_initial(p) || isdigit(p); }
int is_reserved(char *p, char *reserved) {
  int length = strlen(reserved);
  if (start_with(p, reserved) && !is_identifier(p[length])) {
    return length;
  }

  return 0;
}

Token *tokenize(char *p) {
  user_input = p;

  Token head;
  head.next = NULL;
  Token *current = &head;

  while (*p) {
    //空白文字をスキップ
    if (isspace(*p)) {
      p++;
      continue;
    }

    //---記号---
    if (start_with(p, ">=") || start_with(p, "<=") || start_with(p, "==") ||
        start_with(p, "!=")) {
      current = new_token(TOKEN_RESERVED, current, p, 2);
      p += 2;
      continue;
    }

    if (strchr("+-*/()<>=;{},&", *p)) {
      current = new_token(TOKEN_RESERVED, current, p++, 1);
      continue;
    }
    //----------

    //--予約語--
    //同時に成立しないのでやっているが、最悪ぽい
    int length = is_reserved(p, "if");
    length += is_reserved(p, "else");
    length += is_reserved(p, "while");
    length += is_reserved(p, "for");
    length += is_reserved(p, "return");
    length += is_reserved(p, "int");
    if (length > 0) {
      current = new_token(TOKEN_RESERVED, current, p, length);
      p += length;
      continue;
    }
    //----------

    //--識別子--
    if (is_identifier_initial(*p)) {
      char *q = p + 1;
      while (is_identifier(*q))
        q++;
      current = new_token(TOKEN_IDENTIFIER, current, p, q - p);
      p = q;
      continue;
    }
    //----------

    //---数値---
    if (isdigit(*p)) {
      current = new_token(TOKEN_NUMBER, current, p, 0);
      char *q = p;
      current->value = strtol(p, &p, 10);
      current->length = p - q;
      continue;
    }
    //----------

    error_at(p, "トークナイズできません");
  }

  new_token(TOKEN_EOF, current, p, 0);
  return head.next;
}
