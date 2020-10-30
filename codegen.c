#include "9cc.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void generate_local_variable(Node *node) {
  if (node->kind != NODE_LVAR)
    error("代入の左辺値が変数ではありません");

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
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
void generate_code(ListNode *listNode) {
  //アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  //プロローグ
  //変数26個分の領域を確保
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, %d\n", 26 * 8);

  int labelCount = 0;
  while (listNode) {
    //抽象構文木を降りながらコード生成
    generate_statement(listNode->body, &labelCount);
    listNode = listNode->next;

    //式の評価結果をスタックからポップしてraxに格納
    //スタック溢れ対策も兼ねている
    printf("  pop rax\n");
  }

  //エピローグ
  //最後の式の評価結果はraxに格納済なので、それが戻り値となる
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
}
