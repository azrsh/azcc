#include "container.h"
#include "node.h"
#include "parse.h"
#include "statement.h"
#include "type.h"
#include "util.h"
#include "variable.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INSERT_COMMENT

const char *argumentRegister[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

void insert_comment(char *fmt, ...) {
#ifdef INSERT_COMMENT
  va_list ap;
  va_start(ap, fmt);

  printf("# ");
  vprintf(fmt, ap);
  printf("\n");

  va_end(ap);
#endif
}

void generate_expression(Node *node, int *labelCount);
void generate_variable(Node *node, int *labelCount);
void generate_fuction_call(Node *node, int *labelCount);

void generate_variable(Node *node, int *labelCount) {
  if (node->kind != NODE_VAR)
    error("変数ではありません");

  const Variable *variable = node->variable;
  const char *name = string_to_char(variable->name);
  switch (variable->kind) {
  case VARIABLE_LOCAL:
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", variable->offset);
    printf("  push rax\n");
    return;
  case VARIABLE_GLOBAL:
    printf("  lea rax, %s[rip]\n", name);
    printf("  push rax\n");
    return;
  }
}

void generate_assign_lhs(Node *node, int *labelCount) {
  if (node->kind == NODE_VAR) {
    generate_variable(node, labelCount);
    return;
  }

  if (node->kind == NODE_DEREF) {
    generate_expression(node->lhs, labelCount);
    return;
  }

  error("代入の左辺として予期しないノードが指定されました");
}

void generate_fuction_call(Node *node, int *labelCount) {
  if (node->kind != NODE_FUNC)
    error("関数ではありません");

  const char *functionName = string_to_char(node->functionCall->name);

  insert_comment("function call start : %s", functionName);

  Vector *arguments = node->functionCall->arguments;
  const int currentLabel = *labelCount;
  *labelCount += 1;

  //アライメント
  //スタックの使用予測に基づいてアライメント
  const int stackUnitLength = 8;
  const int alignmentLength = stackUnitLength * 2;
  printf("  push 0\n");
  printf("  mov rax, rsp\n");
  if (vector_length(arguments) > 6) {
    printf("  sub rax, %d\n", (vector_length(arguments) - 6) * stackUnitLength);
  }
  printf("  and rax, %d\n", alignmentLength - 1);
  printf("  jz .Lcall%d\n", currentLabel);
  printf("  push 1\n");
  printf(".Lcall%d:\n", currentLabel);

  //引数の評価
  for (int i = vector_length(arguments) - 1; i >= 0; i--) {
    Node *argument = vector_get(arguments, i);
    generate_expression(argument, labelCount);
  }

  //引数の評価中に関数の呼び出しが発生してレジスタが破壊される可能性があるので
  //引数を全て評価してからレジスタに割り当て
  for (int i = 0; i < 6 && i < vector_length(arguments); i++) {
    printf("  pop %s\n", argumentRegister[i]);
  }

  printf("  mov rax, 0\n");
  printf("  call %s\n", functionName);

  //スタックに積んだ引数を処理
  if (vector_length(arguments) > 6) {
    printf("  add rsp, %d\n", (vector_length(arguments) - 6) * stackUnitLength);
  }

  //アライメントの判定とスタックの復元
  printf("  pop rbx\n");
  printf("  cmp rbx, 0\n");
  printf("  je .Lend%d\n", currentLabel);
  printf("  add rsp, %d\n", stackUnitLength);
  printf(".Lend%d:\n", currentLabel);

  printf("  push rax\n");

  insert_comment("function call end : %s", functionName);
}

void generate_cast(Node *node, int *labelCount) {
  insert_comment("cast start");

  Type *source = node->lhs->type;
  Type *dest = node->type;
  printf("  pop rax\n");

  if (source->kind == CHAR && dest->kind == INT) {
    printf("  movsx rax, al\n");
  } else if (source->kind == INT && dest->kind == CHAR) {
    // printf("  shl rax, %d\n", 64 - 8);
    // printf("  shr rax, %d\n", 64 - 8);
  } else if (source->kind == ARRAY && dest->kind == PTR) {
    generate_variable(node, labelCount);
  }
  printf("  push rax\n");

  insert_comment("cast end");
}

void generate_assign_i64_to_i64(Node *node, int *labelCount) {
  printf("  pop rdi\n");
  printf("  pop rax\n");
  printf("  mov [rax], rdi\n");
  printf("  push rdi\n");
}

void generate_assign_i8_to_i8(Node *node, int *labelCount) {
  printf("  pop rdi\n");
  printf("  pop rax\n");
  printf("  mov BYTE PTR [rax], dil\n");
  printf("  push rdi\n");
}

void generate_expression(Node *node, int *labelCount) {
  switch (node->kind) {
  case NODE_NUM:
    printf("  push %d\n", node->val);
    return;
  case NODE_STRING:
    //処理はグローバル変数と同じなので共通化できそう
    printf("  lea rax, .LC%d[rip]\n", node->val);
    printf("  push rax\n");
    return;
  case NODE_REF:
    generate_variable(node->lhs, labelCount);
    return;
  case NODE_DEREF:
    generate_expression(node->lhs, labelCount);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case NODE_VAR:
    generate_variable(node, labelCount);

    //配列の暗黙的なキャスト
    if (node->type->kind != ARRAY) {
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
    }
    return;
  case NODE_FUNC:
    generate_fuction_call(node, labelCount);
    return;
  case NODE_ASSIGN:
    insert_comment("assign start");

    insert_comment("assign lhs start");
    generate_assign_lhs(node->lhs, labelCount);
    insert_comment("assign lhs end");
    insert_comment("assign rhs start");
    generate_expression(node->rhs, labelCount);
    insert_comment("assign rhs end");

    size_t lhsSize = type_to_size(node->lhs->type);
    size_t rhsSize = type_to_size(node->rhs->type);
    if (lhsSize != rhsSize)
      error("右辺を左辺と同じ型にキャストできない不正な代入です");

    if (lhsSize == 1 && rhsSize == 1)
      generate_assign_i8_to_i8(node, labelCount);
    else
      generate_assign_i64_to_i64(node, labelCount);

    insert_comment("assign end");
    return;
  case NODE_CAST:
    generate_expression(node->lhs, labelCount);
    generate_cast(node, labelCount);
    return;
  }

  generate_expression(node->lhs, labelCount);
  generate_expression(node->rhs, labelCount);
  Type *lhsType = node->lhs->type;
  Type *rhsType = node->rhs->type;

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case NODE_ADD:
    insert_comment("start add node");

    // int+int、pointer+intのみを許可する
    Type *addLhsPointerTo = node->lhs->type->base;
    if (addLhsPointerTo) {
      printf("  imul rdi, %d\n", type_to_size(addLhsPointerTo));
    }

    printf("  add rax, rdi\n");
    insert_comment("end add node");
    break;
  case NODE_SUB:
    insert_comment("start sub node");

    // int-int、pointer-intのみを許可する
    Type *subLhsPointerTo = node->lhs->type->base;
    if (subLhsPointerTo) {
      printf("  imul rdi, %d\n", type_to_size(subLhsPointerTo));
    }

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

void generate_string_literal(int index, const char *string) {
  printf("  .data\n");
  printf(".LC%d:\n", index);
  printf("  .string \"%s\"\n", string);
}

void generate_global_variable(const Variable *globalVariable) {
  if (globalVariable->kind != VARIABLE_GLOBAL)
    error("グローバル変数ではありません");

  const char *name = string_to_char(globalVariable->name);
  const size_t typeSize = type_to_size(globalVariable->type);
  printf("  .globl %s\n", name);
  printf("  .data\n");
  printf("%s:\n", name);
  printf("  .zero %zu\n", typeSize);
}

void generate_statement(StatementUnion *statementUnion, int *labelCount) {
  // match if
  {
    IfStatement *ifPattern = statement_union_take_if(statementUnion);
    if (ifPattern) {
      int ifLabel = *labelCount;
      *labelCount += 1;

      generate_expression(ifPattern->condition, labelCount);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");

      if (ifPattern->elseStatement) {
        printf("  je .Lelse%d\n", ifLabel);
      } else {
        printf("  je .Lend%d\n", ifLabel);
      }

      generate_statement(ifPattern->thenStatement, labelCount);

      if (ifPattern->elseStatement) {
        printf(".Lelse%d:\n", ifLabel);
        generate_statement(ifPattern->elseStatement, labelCount);
      }

      printf(".Lend%d:\n", ifLabel);
      return;
    }
  }

  // match while
  {
    WhileStatement *whilePattern = statement_union_take_while(statementUnion);
    if (whilePattern) {
      int loopLabel = *labelCount;
      *labelCount += 1;

      printf("begin%d:\n", loopLabel);

      generate_expression(whilePattern->condition, labelCount);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je end%d\n", loopLabel);

      generate_statement(whilePattern->statement, labelCount);
      printf("  jmp begin%d\n", loopLabel);

      printf("end%d:\n", loopLabel);
      return;
    }
  }

  // match for
  {
    ForStatement *forPattern = statement_union_take_for(statementUnion);
    if (forPattern) {
      int loopLabel = *labelCount;
      *labelCount += 1;

      generate_expression(forPattern->initialization, labelCount);

      printf("begin%d:\n", loopLabel);

      generate_expression(forPattern->condition, labelCount);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je end%d\n", loopLabel);

      generate_statement(forPattern->statement, labelCount);
      generate_expression(forPattern->afterthought, labelCount);
      printf("  jmp begin%d\n", loopLabel);

      printf("end%d:\n", loopLabel);
      return;
    }
  }

  // match compound
  {
    CompoundStatement *compoundPattern =
        statement_union_take_compound(statementUnion);
    if (compoundPattern) {
      ListNode *node = compoundPattern->statementHead;

      while (node) {
        generate_statement(node->body, labelCount);
        node = node->next;
      }

      return;
    }
  }

  // match return
  {
    ReturnStatement *returnPattern =
        statement_union_take_return(statementUnion);
    if (returnPattern) {
      generate_expression(returnPattern->node, labelCount);
      printf("  pop rax\n");
      printf("  mov rsp, rbp\n");
      printf("  pop rbp\n");
      printf("  ret\n");
      return;
    }
  }

  // match expression
  {
    ExpressionStatement *expressionPattern =
        statement_union_take_expression(statementUnion);
    if (expressionPattern) {
      generate_expression(expressionPattern->node, labelCount);
      return;
    }
  }
}

//抽象構文木をもとにコード生成を行う
void generate_function_definition(const FunctionDefinition *functionDefinition,
                                  int *labelCount) {
  const char *functionName = string_to_char(functionDefinition->name);

  //ラベルを生成
  printf("  .text\n");
  printf("  .global %s\n", functionName);
  // macは先頭に_を挿入するらしい
  printf("%s:\n", functionName);

  //プロローグ
  //変数26個分の領域を確保
  insert_comment("function prologue start : %s", functionName);
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, %zu\n", functionDefinition->stackSize);
  insert_comment("function prologue end : %s", functionName);

  //引数の代入処理
  insert_comment("function arguments assign start : %s", functionName);
  int argumentStackOffset = 0;
  for (int i = vector_length(functionDefinition->arguments) - 1; i >= 0; i--) {
    Node *node = vector_get(functionDefinition->arguments, i);
    generate_variable(node, labelCount);
    printf("  pop rax\n");

    if (i < 6) {
      printf("  mov [rax], %s\n", argumentRegister[i]);
    } else {
      printf("  mov rbx, [rbp+%d]\n", 16 + argumentStackOffset);
      argumentStackOffset += type_to_stack_size(node->type);
      printf("  mov [rax], rbx\n");
    }
  }
  insert_comment("function arguments assign end : %s", functionName);

  insert_comment("function body start : %s", functionName);
  ListNode *statementList = functionDefinition->body->statementHead;
  while (statementList) {
    insert_comment("statement start");

    //抽象構文木を降りながらコード生成
    generate_statement(statementList->body, labelCount);
    statementList = statementList->next;

    //文の評価結果をスタックからポップしてraxに格納
    //スタック溢れ対策も兼ねている
    printf("  pop rax\n");

    insert_comment("statement end");
  }
  insert_comment("function body end : %s", functionName);

  //エピローグ
  //最後の式の評価結果はraxに格納済なので、それが戻り値となる
  insert_comment("function epilogue start : %s", functionName);
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  insert_comment("function epilogue end : %s", functionName);
}

//抽象構文木をもとにコード生成を行う
void generate_code(Program *program) {
  //アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");

  for (int i = 0; i < vector_length(program->stringLiterals); i++) {
    const char *string = vector_get(program->stringLiterals, i);
    generate_string_literal(i, string);
  }

  for (int i = 0; i < vector_length(program->globalVariables); i++) {
    const Variable *variable = vector_get(program->globalVariables, i);
    generate_global_variable(variable);
  }

  int labelCount = 0;
  for (int i = 0; i < vector_length(program->functions); i++) {
    const FunctionDefinition *function = vector_get(program->functions, i);
    generate_function_definition(function, &labelCount);
  }
}
