#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool start_with(char *p, char *q);

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
char *string_to_char(String source);
bool string_compare(String string1, String string2);

typedef struct ListNode ListNode;
struct ListNode {
  void *body;
  ListNode *next;
};
ListNode *new_list_node(void *body);
ListNode *list_head_to_tail(ListNode *head);
void list_merge(ListNode *list1, ListNode *list2);
ListNode *list_push_back(ListNode *tail, void *element);
ListNode *list_push_front(ListNode *head, void *element);

typedef struct Vector Vector;
Vector *new_vector(int initialSize);
void vector_push_back(Vector *vector, void *element);
void *vector_get(Vector *vector, int index);
int vector_length(Vector *vector);

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
typedef enum { INT, PTR, ARRAY } TypeKind;
typedef struct Type Type;
struct Type {
  TypeKind kind;
  Type *base;
  size_t size; // TypeKindがARRAYのときのみ使用する
};

typedef struct LocalVariable LocalVariable;
struct LocalVariable {
  String name; //名前
  Type *type;  //名前
  int offset;  // RBPからのオフセット
};

typedef struct FunctionCall FunctionCall;
struct FunctionCall {
  Type *type;        //戻り値の型
  String name;       //名前
  Vector *arguments; //引数の連結リスト, Node
};

typedef enum {
  NODE_ADD,    // +
  NODE_SUB,    // -
  NODE_MUL,    // *
  NODE_DIV,    // /
  NODE_EQ,     // ==
  NODE_NE,     // !=
  NODE_LT,     // <
  NODE_LE,     // <=
  NODE_REF,    // &
  NODE_DEREF,  // *
  NODE_IF,     // if文
  NODE_ASSIGN, // 代入
  NODE_LVAR,   // ローカル変数
  NODE_FUNC,   // 関数
  NODE_ARG,    // 関数の引数
  NODE_NUM     // 整数
} NodeKind;

typedef struct Node Node;
struct Node {
  NodeKind kind; //ノードの型
  Node *lhs;
  Node *rhs;
  int val;                    // kindがNODE_NUMのときのみ使う
  int offset;                 // kindがNODE_LVARのときのみ使う
  Type *type;                 // kindがNODE_LVARのときのみ使う
  FunctionCall *functionCall; // kindがNODE_FUNCのときのみ使う
};

typedef struct VariableContainer VariableContainer;
VariableContainer *new_variable_container(ListNode *tableHead);
LocalVariable *variable_container_get(VariableContainer *container,
                                      String name);
bool variable_container_push(VariableContainer *container,
                             LocalVariable *varibale);
bool variable_container_update(VariableContainer *container,
                               LocalVariable *variable);
VariableContainer *variable_container_push_table(VariableContainer *container,
                                                 HashTable *table);

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
  Node *condition;
  StatementUnion *thenStatement;
  StatementUnion *elseStatement;
};

typedef struct WhileStatement WhileStatement;
struct WhileStatement {
  Node *condition;
  StatementUnion *statement;
};

typedef struct ForStatement ForStatement;
struct ForStatement {
  Node *initialization;
  Node *condition;
  Node *afterthought;
  StatementUnion *statement;
};

typedef struct CompoundStatement CompoundStatement;
struct CompoundStatement {
  ListNode *statementHead;
};

ExpressionStatement *
statement_union_take_expression(StatementUnion *statementUnion);
ReturnStatement *statement_union_take_return(StatementUnion *statementUnion);
IfStatement *statement_union_take_if(StatementUnion *statementUnion);
WhileStatement *statement_union_take_while(StatementUnion *statementUnion);
ForStatement *statement_union_take_for(StatementUnion *statementUnion);
CompoundStatement *
statement_union_take_compound(StatementUnion *statementUnion);

typedef struct FunctionDefinition FunctionDefinition;
struct FunctionDefinition {
  String name;
  ListNode *variableContainer; // LocalVariable HashTable vector
  Vector *arguments;           // Local Variavble Nodes
  CompoundStatement *body;
};

ListNode *parse(Token *head);

//
//型検査器
//
void tag_type_to_node(Node *node);

//
//コードジェネレータ
//
//抽象構文木をもとにコード生成を行う
void generate_code(ListNode *node);
