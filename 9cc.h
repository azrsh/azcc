#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
//コンテナ
//
typedef struct String String;
struct String {
  char *head;
  int length;
};
String new_string(char *source, int length);
String char_to_string(char *source);
bool string_compare(String string1, String string2);

typedef struct ListNode ListNode;
struct ListNode {
  void *body;
  ListNode *next;
};

ListNode *new_list_node(void *body, ListNode *next);

typedef struct HashTable HashTable;
HashTable *new_hash_table();
int hash_table_store(HashTable *table, String key, void *data);
bool hash_table_contain(HashTable *table, String key);
void *hash_table_find(HashTable *table, String key);

//
//トークナイザ
//
typedef enum {
  TOKEN_RESERVED,   //記号
  TOKEN_IDENTIFIER, //識別子
  TOKEN_NUMBER,     //整数
  TOKEN_EOF         //入力の終端
} TokenKind;

typedef struct Token Token;
struct Token {
  TokenKind kind; //トークンの種類
  Token *next;    //次の入力トークン
  int value;      // kindがTOKEN_NUMBERの場合、その値
  char *string;   //トークン文字列
  int length;     //トークン文字列の長さ
};

void error_at(char *location, char *fmt, ...);

//エラーを報告するための関数
// printfと同じ引数をとる
void error(char *fmt, ...);

Token *tokenize(char *p);

//
//パーサ
//
typedef enum {
  NODE_ADD,    // +
  NODE_SUB,    // -
  NODE_MUL,    // *
  NODE_DIV,    // /
  NODE_EQ,     // ==
  NODE_NE,     // !=
  NODE_LT,     // <
  NODE_LE,     // <=
  NODE_IF,     // if文
  NODE_ASSIGN, // 代入
  NODE_LVAR,   // ローカル変数
  NODE_NUM     // 整数
} NodeKind;

typedef struct Node Node;
struct Node {
  NodeKind kind; //ノードの型
  Node *lhs;
  Node *rhs;
  int val;    // kindがNODE_NUMのときのみ使う
  int offset; // kindがNODE_LVARのときのみ使う
};

typedef struct LocalVariable LocalVariable;
struct LocalVariable {
  String name; //名前
  int offset;  // RBPからのオフセット
};

typedef struct StatementUnion StatementUnion;

typedef struct ExpressionStatement ExpressionStatement;
struct ExpressionStatement {
  Node *node;
};

typedef struct ReturnStatement ReturnStatement;
struct ReturnStatement {
  Node *node;
};

typedef struct IfStatement IfStatement;
struct IfStatement {
  Node *conditionExpression;
  StatementUnion *thenStatement;
  StatementUnion *elseStatement;
};

ExpressionStatement *
statement_union_take_expression(StatementUnion *statementUnion);
ReturnStatement *statement_union_take_return(StatementUnion *statementUnion);
IfStatement *statement_union_take_if(StatementUnion *statementUnion);

ListNode *parse(Token *head);

//
//コードジェネレータ
//
//抽象構文木をもとにコード生成を行う
void generate_code(ListNode *node);
