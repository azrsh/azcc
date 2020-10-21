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
  int length;     //トークン文字列の長さ
};

//現在着目しているトークン
Token *token;

typedef enum {
  NODE_ADD, // +
  NODE_SUB, // -
  NODE_MUL, // *
  NODE_DIV, // /
  NODE_EQ,  // ==
  NODE_NE,  // !=
  NODE_LT,  // <
  NODE_LE,  // <=
  NODE_NUM  // 整数
} NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind; //ノードの型
  Node *lhs;
  Node *rhs;
  int val; // kindがNODE_NUMのときのみ使う
};

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
bool consume(char *op) {
  if (token->kind != TOKEN_RESERVED || strlen(op) != token->length ||
      memcmp(token->string, op, token->length))
    return false;
  token = token->next;
  return true;
}

//次のトークンが期待している記号のときには、トークンを1つ読み進める
//それ以外の場合にはエラーを報告する
void expect(char *op) {
  if (token->kind != TOKEN_RESERVED || strlen(op) != token->length ||
      memcmp(token->string, op, token->length))
    error("'%c'ではありません", op);
  token = token->next;
}

//次のトークンが数値のときには、トークンを1つ読み進めてその数値を返す
//それ以外の場合にはエラーを報告する
int expect_number() {
  if (token->kind != TOKEN_NUMBER)
    error_at(token->string, "数ではありません");
  int value = token->value;
  token = token->next;
  return value;
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

//抽象構文木の数値以外のノードを新しく生成する
Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

//抽象構文木の数値のノードを新しく生成する
Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = NODE_NUM;
  node->val = val;
  return node;
}

Node *expression();
Node *equality();
Node *relational();
Node *add();
Node *multiply();
Node *unary();
Node *primary();

// expression = equality
// equality   = relational ("==" relational | "!=" relational)*
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
// add        = mul ("+" mul | "-" mul)*
// mul        = unary ("*" unary | "/" unary)*
// unary      = ("+" | "-")? primary
// primary    = num | "(" expr ")"

//式をパースする
Node *expression() { return equality(); }

//等式をパースする
Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume("=="))
      node = new_node(NODE_EQ, node, relational());
    else if (consume("!="))
      node = new_node(NODE_NE, node, relational());
    else
      return node;
  }
}

//不等式をパースする
Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume("<"))
      node = new_node(NODE_LT, node, add());
    else if (consume("<="))
      node = new_node(NODE_LE, node, add());
    else if (consume(">"))
      node = new_node(NODE_LT, add(), node);
    else if (consume(">="))
      node = new_node(NODE_LE, add(), node);
    else
      return node;
  }
}

Node *add() {
  Node *node = multiply();

  for (;;) {
    if (consume("+"))
      node = new_node(NODE_ADD, node, multiply());
    else if (consume("-"))
      node = new_node(NODE_SUB, node, multiply());
    else
      return node;
  }
}

//乗除算をパースする
Node *multiply() {
  Node *node = unary();

  for (;;) {
    if (consume("*"))
      node = new_node(NODE_MUL, node, unary());
    else if (consume("/"))
      node = new_node(NODE_DIV, node, unary());
    else
      return node;
  }
}

//単項演算子をパースする
Node *unary() {
  if (consume("+"))
    return primary();
  if (consume("-"))
    return new_node(NODE_SUB, new_node_num(0), primary());
  return primary();
}

//抽象構文木の末端をパースする
Node *primary() {
  //次のトークンが(なら入れ子になった式
  if (consume("(")) {
    Node *node = expression();
    expect(")");
    return node;
  }

  //そうでなければ整数
  return new_node_num(expect_number());
}

//抽象構文木をもとにコード生成を行う
void generate(Node *node) {
  if (node->kind == NODE_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  generate(node->lhs);
  generate(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case NODE_ADD:
    printf("  add rax, rdi\n");
    break;
  case NODE_SUB:
    printf("  sub rax, rdi\n");
    break;
  case NODE_MUL:
    printf("  imul rax, rdi\n");
    break;
  case NODE_DIV:
    // idiv命令のためにraxを符号を維持したままrdx+raxの128ビット整数に伸ばす
    // idiv命令は暗黙のうちにrdx+raxを取る
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case NODE_EQ:
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case NODE_NE:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case NODE_LT:
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case NODE_LE:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  }

  printf("  push rax\n");
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return EXIT_FAILURE;
  }

  //トークナイズしてパースする
  user_input = argv[1];
  token = tokenize(user_input);
  Node *node = expression();

  //アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  //抽象構文木を降りながらコード生成
  generate(node);

  //スタックトップに式全体の値が格納されているのでそれをraxにロードして関数の返り値とする
  printf("  pop rax\n");
  printf("  ret\n");
  return EXIT_SUCCESS;
}
