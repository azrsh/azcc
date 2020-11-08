#include "9cc.h"
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

  va_end(ap);
#endif
}

void generate_expression(Node *node);
void generate_local_variable(Node *node);
void generate_fuction_call(Node *node);

void generate_local_variable(Node *node) {
  if (node->kind != NODE_LVAR)
    error("代入の左辺値が変数ではありません");

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void generate_fuction_call(Node *node) {
  if (node->kind != NODE_FUNC)
    error("関数ではありません");

  const char *functionName = string_to_char(node->functionCall->name);

  insert_comment("function call start : %s\n", functionName);

  Vector *arguments = node->functionCall->arguments;
  for (int i = vector_length(arguments) - 1; i >= 0; i--) {
    Node *argument = vector_get(arguments, i);
    generate_expression(argument);
    if (i < 6) {
      printf("  pop %s\n", argumentRegister[i]);
    }
  }

  printf("  call %s\n", functionName);
  printf("  push rax\n");

  insert_comment("function call end : %s\n", functionName);
}

void generate_expression(Node *node) {
  switch (node->kind) {
  case NODE_NUM:
    printf("  push %d\n", node->val);
    return;
  case NODE_LVAR:
    generate_local_variable(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case NODE_FUNC:
    generate_fuction_call(node);
    return;
  case NODE_ASSIGN:
    generate_local_variable(node->lhs);
    generate_expression(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    return;
  }

  generate_expression(node->lhs);
  generate_expression(node->rhs);

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

void generate_statement(StatementUnion *statementUnion, int *labelCount) {
  // match if
  {
    IfStatement *ifPattern = statement_union_take_if(statementUnion);
    if (ifPattern) {
      int endLabel = *labelCount;
      int elseLabel = *labelCount + 1;
      *labelCount += 2;

      generate_expression(ifPattern->condition);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");

      if (ifPattern->elseStatement) {
        printf("  je .LabelElse%d\n", elseLabel);
      } else {
        printf("  je .LabelEnd%d\n", endLabel);
      }

      generate_statement(ifPattern->thenStatement, labelCount);

      if (ifPattern->elseStatement) {
        printf(".LabelElse%d:\n", elseLabel);
        generate_statement(ifPattern->elseStatement, labelCount);
      }

      printf(".LabelEnd%d:\n", endLabel);
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

      generate_expression(whilePattern->condition);
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

      generate_expression(forPattern->initialization);

      printf("begin%d:\n", loopLabel);

      generate_expression(forPattern->condition);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je end%d\n", loopLabel);

      generate_statement(forPattern->statement, labelCount);
      generate_expression(forPattern->afterthought);
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
      generate_expression(returnPattern->node);
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
      generate_expression(expressionPattern->node);
      return;
    }
  }
}

//抽象構文木をもとにコード生成を行う
void generate_function_definition(FunctionDefinition *functionDefinition,
                                  int *labelCount) {
  const char *functionName = string_to_char(functionDefinition->name);

  //ラベルを生成
  // macは先頭に_を挿入するらしい
  printf("%s:\n", functionName);

  //プロローグ
  //変数26個分の領域を確保
  insert_comment("function prologue start : %s\n", functionName);
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, %d\n", 26 * 8);
  insert_comment("function prologue end : %s\n", functionName);

  //引数の代入処理
  insert_comment("function arguments assign start : %s\n", functionName);
  for (int i = vector_length(functionDefinition->arguments) - 1; i >= 0; i--) {
    Node *node = vector_get(functionDefinition->arguments, i);
    generate_local_variable(node);
    printf("  pop rax\n");

    if (i < 6) {
      printf("  mov [rax], %s\n", argumentRegister[i]);
    } else {
      printf("  mov rbx, [rbp+%d]\n", (i - 4) * 8);
      printf("  mov [rax], rbx\n");
    }
  }
  insert_comment("function arguments assign end : %s\n", functionName);

  insert_comment("function arguments body start : %s\n", functionName);
  ListNode *statementList = functionDefinition->body->statementHead;
  while (statementList) {
    //抽象構文木を降りながらコード生成
    generate_statement(statementList->body, labelCount);
    statementList = statementList->next;

    //文の評価結果をスタックからポップしてraxに格納
    //スタック溢れ対策も兼ねている
    printf("  pop rax\n");
  }
  insert_comment("function arguments body end : %s\n", functionName);

  //エピローグ
  //最後の式の評価結果はraxに格納済なので、それが戻り値となる
  insert_comment("function arguments epilogue start : %s\n", functionName);
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  insert_comment("function arguments epilogue end : %s\n", functionName);
}

//抽象構文木をもとにコード生成を行う
void generate_code(ListNode *functionDefinitionList) {
  //アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");

  int labelCount = 0;
  while (functionDefinitionList) {
    generate_function_definition(functionDefinitionList->body, &labelCount);
    functionDefinitionList = functionDefinitionList->next;
  }
}
