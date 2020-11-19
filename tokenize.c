#include "tokenize.h"
#include "container.h"
#include "util.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//新しいトークンを作成してcurrentに繋げる
Token *new_token(TokenKind kind, Token *current, const char *string,
                 int length) {
  Token *token = calloc(1, sizeof(Token));
  token->kind = kind;
  token->string = new_string(string, length);
  current->next = token;
  return token;
}

bool is_identifier_initial(char p) { return isalpha(p) || p == '_'; }
bool is_identifier(char p) { return is_identifier_initial(p) || isdigit(p); }
int is_reserved(const char *p, const char *reserved) {
  int length = strlen(reserved);
  if (start_with(p, reserved) && !is_identifier(p[length])) {
    return length;
  }

  return 0;
}

Token *tokenize(const char *p) {
  user_input = p;

  Token head;
  head.next = NULL;
  Token *current = &head;

  while (*p) {
    //---スキップ---
    //空白文字をスキップ
    if (isspace(*p)) {
      p++;
      continue;
    }

    // 行コメントをスキップ
    if (strncmp(p, "//", 2) == 0) {
      p += 2;
      while (*p != '\n')
        p++;
      continue;
    }

    // ブロックコメントをスキップ
    if (strncmp(p, "/*", 2) == 0) {
      char *q = strstr(p + 2, "*/");
      if (!q)
        error_at(p, "コメントが閉じられていません");
      p = q + 2;
      continue;
    }
    //--------------

    //---記号---
    if (start_with(p, ">=") || start_with(p, "<=") || start_with(p, "==") ||
        start_with(p, "!=")) {
      current = new_token(TOKEN_RESERVED, current, p, 2);
      p += 2;
      continue;
    }

    if (strchr("+-*/()<>=;{},&[]", *p)) {
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
    length += is_reserved(p, "char");
    length += is_reserved(p, "sizeof");
    if (length > 0) {
      current = new_token(TOKEN_RESERVED, current, p, length);
      p += length;
      continue;
    }
    //----------

    //--識別子--
    if (is_identifier_initial(*p)) {
      const char *q = p + 1;
      while (is_identifier(*q))
        q++;
      current = new_token(TOKEN_IDENTIFIER, current, p, q - p);
      p = q;
      continue;
    }
    //----------

    //--文字列--
    if (strchr("\"", *p)) {
      p++;
      const char *q = p;
      while (!strchr("\"", *p)) {
        p++;
      }
      current = new_token(TOKEN_STRING, current, q, p - q);
      p++;
      continue;
    }
    //----------

    //---数値---
    if (isdigit(*p)) {
      const char *q = p;
      char *end;
      int value = strtol(p, &end, 10);
      int length = end - q;
      current = new_token(TOKEN_NUMBER, current, p, length);
      current->value = value;

      p += length;
      continue;
    }
    //----------

    error_at(p, "トークナイズできません");
  }

  new_token(TOKEN_EOF, current, p, 0);
  return head.next;
}
