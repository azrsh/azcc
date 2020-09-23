#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  TOKEN_RESERVED, //記号
  TOKEN_NUMBER,   //整数
  TOKEN_EOF       //入力の終端
} TokenKind;

typedef struct Token Token;
struct Token {
  TokenKind kind; //トークンの種類
  Token *next;    //次の入力トークン
  int value;      // kindがTOKEN_NUMBERの場合、その値
  char *string;   //トークン文字列
};

//現在着目しているトークン
Token *token;

//エラーを報告するための関数
// printfと同じ引数をとる
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(EXIT_FAILURE);
}

//次のトークンが期待している記号のときには、トークンを1つ読み進めて真を返す
//それ以外の場合には偽を返す
bool consume(char op) {
  if (token->kind != TOKEN_RESERVED || token->string[0] != op)
    return false;
  token = token->next;
  return true;
}

//次のトークンが期待している記号のときには、トークンを1つ読み進める
//それ以外の場合にはエラーを報告する
void expect(char op) {
  if (token->kind != TOKEN_RESERVED || token->string[0] != op)
    error("'%c'ではありません", op);
  token = token->next;
}

//次のトークンが数値のときには、トークンを1つ読み進めてその数値を返す
//それ以外の場合にはエラーを報告する
int expect_number() {
  if (token->kind != TOKEN_NUMBER)
    error("数ではありません");
  int value = token->value;
  token = token->next;
  return value;
}

bool at_eof() { return token->kind == TOKEN_EOF; }

//新しいトークンを作成してcurrentに繋げる
Token *new_token(TokenKind kind, Token *current, char *string) {
  Token *token = calloc(1, sizeof(Token));
  token->kind = kind;
  token->string = string;
  current->next = token;
  return token;
}

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

    if (*p == '+' || *p == '-') {
      current = new_token(TOKEN_RESERVED, current, p++);
      continue;
    }

    if (isdigit(*p)) {
      current = new_token(TOKEN_NUMBER, current, p);
      current->value = strtol(p, &p, 10);
      continue;
    }

    error("トークナイズできません");
  }

  new_token(TOKEN_EOF, current, p);
  return head.next;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return EXIT_FAILURE;
  }

  token = tokenize(argv[1]);

  //アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  //式の最初は数でなければならないので、それをチェックして最初のmov命令を出力
  printf("  mov rax, %d\n", expect_number());

  while (!at_eof()) {
    if (consume('+')) {
      printf("  add rax, %d\n", expect_number());
      continue;
    }

    expect('-');
    printf("  sub rax, %d\n", expect_number());
    continue;
  }

  printf("  ret\n");
  return EXIT_SUCCESS;
}
