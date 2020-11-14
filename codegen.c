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
  printf("\n");

  va_end(ap);
#endif
}

void generate_expression(Node *node, int *labelCount);
void generate_local_variable(Node *node, int *labelCount);
void generate_fuction_call(Node *node, int *labelCount);

void generate_local_variable(Node *node, int *labelCount) {
  if (node->kind != NODE_LVAR)
    error("変数ではありません");

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void generate_assign_lhs(Node *node, int *labelCount) {
  if (node->kind == NODE_LVAR) {
    generate_local_variable(node, labelCount);
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

  //アライメント
  if (vector_length(arguments) > 6) {
    //スタックの使用予測に基づいてアライメント
    const int stackUnitLength = 8;
    const int alignmentLength = stackUnitLength * 2;
    printf("  mov rax, rsp\n");
    printf("  sub rax, %d\n", (vector_length(arguments) - 6) * 8);
    printf("  and rax, %d\n", alignmentLength - 1);
    printf("  jz .Lcall%d\n", *labelCount);
    printf("  sub rsp, %d\n", stackUnitLength);
    printf(".Lcall%d:\n", *labelCount);
  }

  for (int i = vector_length(arguments) - 1; i >= 0; i--) {
    Node *argument = vector_get(arguments, i);
    generate_expression(argument, labelCount);
    if (i < 6) {
      printf("  pop %s\n", argumentRegister[i]);
    }
  }

  printf("  mov rax, 0\n");
  printf("  call %s\n", functionName);

  //アライメントの判定とスタックの復元
  if (vector_length(arguments) > 6) {
    const int stackUnitLength = 8;
    const int alignmentLength = stackUnitLength * 2;
    printf("  mov rbx, rsp\n");
    printf("  and rbx, %d\n", alignmentLength - 1);
    printf("  jz .Lend%d\n", *labelCount);
    printf("  add rsp, %d\n", stackUnitLength);
    printf(".Lend%d:\n", *labelCount);

    *labelCount += 1;
  }

  printf("  push rax\n");

  insert_comment("function call end : %s", functionName);
}

void generate_expression(Node *node, int *labelCount) {
  switch (node->kind) {
  case NODE_NUM:
    printf("  push %d\n", node->val);
    return;
  case NODE_REF:
    generate_local_variable(node->lhs, labelCount);
    return;
  case NODE_DEREF:
    generate_expression(node->lhs, labelCount);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case NODE_LVAR:
    generate_local_variable(node, labelCount);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
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

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");

    insert_comment("assign end");
    return;
  }

  generate_expression(node->lhs, labelCount);
  generate_expression(node->rhs, labelCount);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case NODE_ADD:
    insert_comment("start add node");
    int typeSize = 1;
    if (node->lhs->kind == NODE_LVAR && node->lhs->type->kind == PTR) {
      Type *pointerTo = node->lhs->type->pointerTo;
      switch (pointerTo->kind) {
      case PTR:
        typeSize = 8;
        insert_comment("recieved %s", "pointer");
        break;
      case INT:
        typeSize = 4;
        insert_comment("recieved %s", "int");
        break;
      default:
        error("足し算の左辺に予期しないノードが指定されました");
      }
    }

    printf("  imul rdi, %d\n", typeSize);
    printf("  add rax, rdi\n");
    insert_comment("end add node");
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
void generate_function_definition(FunctionDefinition *functionDefinition,
                                  int *labelCount) {
  const char *functionName = string_to_char(functionDefinition->name);

  //ラベルを生成
  // macは先頭に_を挿入するらしい
  printf("%s:\n", functionName);

  //プロローグ
  //変数26個分の領域を確保
  insert_comment("function prologue start : %s", functionName);
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, %d\n", 26 * 8);
  insert_comment("function prologue end : %s", functionName);

  //引数の代入処理
  insert_comment("function arguments assign start : %s", functionName);
  for (int i = vector_length(functionDefinition->arguments) - 1; i >= 0; i--) {
    Node *node = vector_get(functionDefinition->arguments, i);
    generate_local_variable(node, labelCount);
    printf("  pop rax\n");

    if (i < 6) {
      printf("  mov [rax], %s\n", argumentRegister[i]);
    } else {
      printf("  mov rbx, [rbp+%d]\n", (i - 4) * 8);
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
