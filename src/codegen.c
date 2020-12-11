#include "container.h"
#include "node.h"
#include "parse.h"
#include "statement.h"
#include "type.h"
#include "util.h"
#include "variable.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *argumentRegister[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

void generate_expression(Node *node, int *labelCount);
void generate_variable(Node *node);
void generate_function_call(Node *node, int *labelCount);
void generate_assign_lhs(Node *node, int *labelCount);
void generate_value_extension(Node *node);

void generate_variable(Node *node) {
  if (node->kind != NODE_VAR)
    ERROR_AT(node->source, "変数ではありません");

  const Variable *variable = node->variable;
  const char *name = string_to_char(variable->name);
  switch (variable->kind) {
  case VARIABLE_LOCAL:
    printf("  lea rax, [rbp-%d]\n", variable->offset);
    printf("  push rax\n");
    return;
  case VARIABLE_GLOBAL:
    printf("  lea rax, %s[rip]\n", name);
    printf("  push rax\n");
    return;
  case VARIABLE_MEMBER:
  case VARIABLE_ENUMERATOR:
    ERROR_AT(node->source, "予期しない種類の変数です");
    return;
  }
}

void generate_dot_operator(Node *node, int *labelCount) {
  if (node->kind != NODE_DOT)
    ERROR_AT(node->source, "ドット演算子ではありません");

  INSERT_COMMENT("dot operator start");
  if (node->lhs->type->kind != TYPE_STRUCT || node->rhs->kind != NODE_VAR /*||
      node->rhs->variable->kind != VARIABLE_MEMBER*/)
    ERROR_AT(node->source, "ドット演算子のオペランドが不正です");
  generate_assign_lhs(node->lhs, labelCount);
  printf("  pop rax\n");
  printf("  add rax, %d\n", node->rhs->variable->offset);
  printf("  push rax\n");
  INSERT_COMMENT("dot operator end");
}

void generate_assign_lhs(Node *node, int *labelCount) {
  if (node->kind == NODE_VAR) {
    generate_variable(node);
    return;
  }

  if (node->kind == NODE_DEREF) {
    INSERT_COMMENT("dereference before dot start");
    generate_expression(node->lhs, labelCount);
    INSERT_COMMENT("dereference before dot end");
    return;
  }

  if (node->kind == NODE_DOT) {
    generate_dot_operator(node, labelCount);
    return;
  }

  ERROR_AT(node->source, "代入の左辺として予期しないノードが指定されました");
}

void generate_function_call(Node *node, int *labelCount) {
  if (node->kind != NODE_FUNC)
    ERROR("関数ではありません");

  const char *functionName = string_to_char(node->functionCall->name);

  INSERT_COMMENT("function call start : %s", functionName);

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
    INSERT_COMMENT("function %s argument %d start", functionName, i);
    Node *argument = vector_get(arguments, i);
    generate_expression(argument, labelCount);
    INSERT_COMMENT("function %s argument %d end", functionName, i);
  }

  //引数の評価中に関数の呼び出しが発生してレジスタが破壊される可能性があるので
  //引数を全て評価してからレジスタに割り当て
  for (int i = 0; i < 6 && i < vector_length(arguments); i++) {
    printf("  pop %s\n", argumentRegister[i]);
  }

  printf("  mov rax, 0\n");
  // if (string_compare(&node->functionCall->name, char_to_string("calloc")) ||
  //    string_compare(&node->functionCall->name, char_to_string("strlen")) ||
  //    string_compare(&node->functionCall->name, char_to_string("memcmp")) ||
  //    string_compare(&node->functionCall->name, char_to_string("memcpy")))
  //  printf("  call %s@PLT\n", functionName);
  // else
  printf("  call %s\n", functionName);

  //スタックに積んだ引数を処理
  if (vector_length(arguments) > 6) {
    printf("  add rsp, %d\n", (vector_length(arguments) - 6) * stackUnitLength);
  }

  //アライメントの判定とスタックの復元
  printf("  pop r11\n");
  printf("  cmp r11, 0\n");
  printf("  je .Lend%d\n", currentLabel);
  printf("  add rsp, %d\n", stackUnitLength);
  printf(".Lend%d:\n", currentLabel);

  printf("  push rax\n");

  //内部で扱う値は基本的に64bit整数なので拡張
  if (node->type->kind != TYPE_VOID)
    generate_value_extension(node);

  INSERT_COMMENT("function call end : %s", functionName);
}

void generate_cast(Node *node) {
  INSERT_COMMENT("cast start");

  Type *source = node->lhs->type;
  Type *dest = node->type;
  printf("  pop rax\n");

  if (source->kind == TYPE_CHAR && dest->kind == TYPE_INT) {
    INSERT_COMMENT("cast char to int");
    printf("  movsx rax, al\n");
  } else if (source->kind == TYPE_INT && dest->kind == TYPE_CHAR) {
    // 上位56bitを破棄すればよいのでなにもしない
    INSERT_COMMENT("cast int to char");
  } else if (source->kind == TYPE_BOOL && dest->kind == TYPE_INT) {
    INSERT_COMMENT("cast bool to int");
    printf("  movsx rax, al\n");
  } else if (source->kind == TYPE_INT && dest->kind == TYPE_BOOL) {
    INSERT_COMMENT("cast int to bool");
    printf("  cmp rax, 0\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
  } else if (source->kind == TYPE_ARRAY && dest->kind == TYPE_PTR) {
    // generate_variable(node);
    // 今はgenerate_rhd_extensionでやっている
  }

  // void*とポインタ型のキャストを許可
  // 比較演算子におけるポインタ型と0の比較を許可

  printf("  push rax\n");

  INSERT_COMMENT("cast end");
}

void generate_assign_i64() {
  printf("  pop rdi\n");
  printf("  pop rax\n");
  printf("  mov [rax], rdi\n");
  printf("  push rdi\n");
}

void generate_assign_i32() {
  printf("  pop rdi\n");
  printf("  pop rax\n");
  printf("  mov DWORD PTR [rax], edi\n");
  printf("  push rdi\n");
}

void generate_assign_i8() {
  printf("  pop rdi\n");
  printf("  pop rax\n");
  printf("  mov BYTE PTR [rax], dil\n");
  printf("  push rdi\n");
}

void generate_rhs_extension(Node *node) {
  //暗黙的なキャスト
  //配列はポインタに
  //それ以外の値は64bitに符号拡張
  printf("  pop rax\n");
  switch (node->type->kind) {
  case TYPE_CHAR:
    printf("  movsx rax, BYTE PTR [rax]\n");
    break;
  case TYPE_BOOL:
    printf("  movzx rax, BYTE PTR [rax]\n");
    break;
  case TYPE_INT:
  case TYPE_ENUM:
    printf("  movsx rax, DWORD PTR [rax]\n");
    break;
  case TYPE_PTR:
  case TYPE_STRUCT:
    printf("  mov rax, [rax]\n");
    break;
  case TYPE_ARRAY:
    break; //配列はポインタのままにする
  case TYPE_VOID:
    ERROR_AT(node->source, "許可されていない型の値です");
  }
  printf("  push rax\n");
}

void generate_value_extension(Node *node) {
  // 64bitに符号拡張
  printf("  pop rax\n");
  switch (node->type->kind) {
  case TYPE_CHAR:
    INSERT_COMMENT("extension char to i64");
    printf("  movsx rax, al\n");
    break;
  case TYPE_BOOL:
    INSERT_COMMENT("extension bool to i64");
    printf("  movzx rax, al\n");
    break;
  case TYPE_INT:
  case TYPE_ENUM:
    printf("  movsx rax, eax\n");
    break;
  case TYPE_PTR:
  case TYPE_ARRAY: //値になった時点で配列はポインタに変換されていると考えて良い
    break;
  case TYPE_STRUCT:
  case TYPE_VOID:
    ERROR_AT(node->source, "許可されていない型の値です");
  }
  printf("  push rax\n");
}

void generate_expression(Node *node, int *labelCount) {
  switch (node->kind) {
  case NODE_NUM:
  case NODE_CHAR:
    printf("  push %d\n", node->val);
    return;
  case NODE_STRING:
    //処理はグローバル変数と同じなので共通化できそう
    printf("  lea rax, .LC%d[rip]\n", node->val);
    printf("  push rax\n");
    return;
  case NODE_ARRAY:
    ERROR_AT(node->source,
             "ローカル変数の配列による初期化はサポートされていません");
    return;
  case NODE_LNOT:
    INSERT_COMMENT("logic not start");
    generate_expression(node->lhs, labelCount);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    printf("  push rax\n");
    INSERT_COMMENT("logic not end");
    return;
  case NODE_REF:
    INSERT_COMMENT("reference start");
    generate_assign_lhs(node->lhs, labelCount);
    INSERT_COMMENT("reference start");
    return;
  case NODE_DEREF:
    INSERT_COMMENT("dereference start");
    generate_expression(node->lhs, labelCount);
    generate_rhs_extension(node);
    INSERT_COMMENT("dereference end");
    return;
  case NODE_VAR:
    generate_variable(node);
    generate_rhs_extension(node);
    return;
  case NODE_DOT:
    generate_dot_operator(node, labelCount);
    generate_rhs_extension(node);
    return;
  case NODE_FUNC:
    generate_function_call(node, labelCount);
    return;
  case NODE_ASSIGN:
    INSERT_COMMENT("assign start");

    INSERT_COMMENT("assign lhs start");
    generate_assign_lhs(node->lhs, labelCount);
    INSERT_COMMENT("assign lhs end");
    INSERT_COMMENT("assign rhs start");
    generate_expression(node->rhs, labelCount);
    INSERT_COMMENT("assign rhs end");

    size_t lhsSize = type_to_size(node->lhs->type);
    size_t rhsSize = type_to_size(node->rhs->type);
    // if (lhsSize != rhsSize)
    //  ERROR("右辺を左辺と同じ型にキャストできない不正な代入です");

    if (lhsSize == 1 && rhsSize == 1)
      generate_assign_i8();
    else if (lhsSize == 4 && rhsSize == 4)
      generate_assign_i32();
    else if (lhsSize == 8 && rhsSize == 8)
      generate_assign_i64();
    else
      ERROR_AT(node->source, "予期しない代入");

    INSERT_COMMENT("assign end");
    return;
  case NODE_CAST:
    generate_expression(node->lhs, labelCount);
    generate_cast(node);
    return;
  case NODE_LAND:
    INSERT_COMMENT("logic and start");
    int landLabel = *labelCount;
    *labelCount += 1;
    generate_expression(node->lhs, labelCount);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lendland%d\n", landLabel);
    generate_expression(node->rhs, labelCount);
    printf("  pop rax\n");
    printf(".Lendland%d:\n", landLabel);
    printf("  cmp rax, 0\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    printf("  push rax\n");
    INSERT_COMMENT("logic and end");
    return;
  case NODE_LOR:
    INSERT_COMMENT("logic or start");
    int lorLabel = *labelCount;
    *labelCount += 1;
    generate_expression(node->lhs, labelCount);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  jne .Lendlor%d\n", lorLabel);
    generate_expression(node->rhs, labelCount);
    printf("  pop rax\n");
    printf(".Lendlor%d:\n", lorLabel);
    printf("  cmp rax, 0\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    printf("  push rax\n");
    INSERT_COMMENT("logic or end");
    return;
  case NODE_ADD:
  case NODE_SUB:
  case NODE_MUL:
  case NODE_DIV:
  case NODE_MOD:
  case NODE_EQ:
  case NODE_NE:
  case NODE_LT:
  case NODE_LE:
    break; //次のswitch文で判定する
  }

  generate_expression(node->lhs, labelCount);
  generate_expression(node->rhs, labelCount);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case NODE_ADD: {
    INSERT_COMMENT("start add node");

    // int+int、pointer+intのみを許可する
    Type *lhsBase = node->lhs->type->base;
    if (lhsBase) {
      printf("  imul rdi, %d\n", type_to_size(lhsBase));
    }

    printf("  add rax, rdi\n");
    INSERT_COMMENT("end add node");
    break;
  }
  case NODE_SUB: {
    INSERT_COMMENT("start sub node");

    // int-int、pointer-int、pointer-pointerのみを許可する
    Type *lhsBase = node->lhs->type->base;
    Type *rhsBase = node->rhs->type->base;
    if (lhsBase && !rhsBase) {
      printf("  imul rdi, %d\n", type_to_size(lhsBase));
    }

    printf("  sub rax, rdi\n");

    if (lhsBase && rhsBase) {
      printf("  cqo\n");
      printf("  mov rdi, %d\n", type_to_size(lhsBase));
      printf("  idiv rdi\n");
    }

    break;
  }
  case NODE_MUL:
    printf("  imul rax, rdi\n");
    break;
  case NODE_DIV:
    // idiv命令のためにraxを符号を維持したままrdx+raxの128ビット整数に伸ばす
    // idiv命令は暗黙のうちにrdx+raxを取る
    // raxに商、rdxに剰余が代入される
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case NODE_MOD:
    // idiv命令のためにraxを符号を維持したままrdx+raxの128ビット整数に伸ばす
    // idiv命令は暗黙のうちにrdx+raxを取る
    // raxに商、rdxに剰余が代入される
    printf("  cqo\n");
    printf("  idiv rdi\n");
    printf("  mov rax, rdx\n");
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
  case NODE_LAND:
  case NODE_LOR:
  case NODE_LNOT:
  case NODE_REF:
  case NODE_DEREF:
  case NODE_ASSIGN:
  case NODE_VAR:
  case NODE_FUNC:
  case NODE_NUM:
  case NODE_CHAR:
  case NODE_STRING:
  case NODE_ARRAY:
  case NODE_CAST:
  case NODE_DOT:
    ERROR("コンパイラの内部エラー");
  }

  printf("  push rax\n");
}

void generate_string_literal(int index, const char *string) {
  printf("  .data\n");
  printf(".LC%d:\n", index);
  printf("  .string \"%s\"\n", string);
}

void generate_global_variable_initializer(Type *type, Node *initializer) {
  //計算に使用しているのが64bit整数なので8byte確保しないと代入で壊れる
  const size_t typeSize = type_to_stack_size(type);
  if (initializer) {
    switch (initializer->kind) {
    case NODE_STRING:
      if (type->kind == TYPE_PTR && type->base->kind == TYPE_CHAR)
        printf("  .quad .LC%d\n", initializer->val);
      else
        ERROR("指定された型のグローバル変数は初期化できません");
      return;
    case NODE_ARRAY: {
      Vector *elements = initializer->elements;
      for (int i = 0; i < vector_length(elements); i++) {
        Node *node = vector_get(elements, i);
        generate_global_variable_initializer(type->base, node);
      }
      break;
    }
    case NODE_NUM:
    case NODE_CHAR:
      switch (type->kind) {
      case TYPE_CHAR:
      case TYPE_BOOL:
        printf("  .byte %d\n", initializer->val);
        return;
      case TYPE_INT:
        printf("  .long %d\n", initializer->val);
        return;
      default:
        ERROR("指定された型のグローバル変数は初期化できません");
      }
    default:
      ERROR("グローバル変数の初期化に失敗しました");
    }
  } else {
    printf("  .zero %zu\n", typeSize);
  }
}

void generate_global_variable(const Variable *variable) {
  if (variable->kind != VARIABLE_GLOBAL)
    ERROR("グローバル変数ではありません");

  const char *name = string_to_char(variable->name);
  printf("  .globl %s\n", name);
  printf("  .data\n");
  printf("%s:\n", name);
  generate_global_variable_initializer(variable->type,
                                       variable->initialization);
}

void generate_statement(StatementUnion *statementUnion, int *labelCount,
                        int latestBreakTarget, int latestSwitch) {
  // match if
  {
    IfStatement *ifPattern = statement_union_take_if(statementUnion);
    if (ifPattern) {
      int ifLabel = *labelCount;
      *labelCount += 1;

      generate_expression(ifPattern->condition, labelCount);
      generate_value_extension(ifPattern->condition);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");

      if (ifPattern->elseStatement) {
        printf("  je .Lelse%d\n", ifLabel);
      } else {
        printf("  je .Lendif%d\n", ifLabel);
      }

      generate_statement(ifPattern->thenStatement, labelCount,
                         latestBreakTarget, latestSwitch);

      if (ifPattern->elseStatement) {
        printf("  jmp .Lendif%d\n", ifLabel);
        printf(".Lelse%d:\n", ifLabel);
        generate_statement(ifPattern->elseStatement, labelCount,
                           latestBreakTarget, latestSwitch);
      }

      printf(".Lendif%d:\n", ifLabel);
      return;
    }
  }

  // match switch
  {
    SwitchStatement *switchPattern =
        statement_union_take_switch(statementUnion);
    if (switchPattern) {
      int switchLabel = *labelCount;
      *labelCount += 1;

      generate_expression(switchPattern->condition, labelCount);
      generate_value_extension(switchPattern->condition);
      printf("  pop rax\n");

      Vector *labeledStatements = switchPattern->labeledStatements;
      for (int i = 0; i < vector_length(labeledStatements); i++) {
        //定数式はコード生成前に解決され、数値になっていることを期待できる
        LabeledStatement *labeled = vector_get(labeledStatements, i);
        Node *constantExpression = labeled->constantExpression;
        if (constantExpression) { // caseラベルの場合
          int value;
          if (constantExpression->kind == NODE_NUM ||
              constantExpression->kind == NODE_CHAR)
            value = constantExpression->val;
          else if (constantExpression->kind == NODE_VAR &&
                   constantExpression->variable->kind == VARIABLE_GLOBAL &&
                   constantExpression->variable->initialization)
            value = constantExpression->variable->initialization->val;
          else
            ERROR_AT(constantExpression->source, "定数式ではありません");
          printf("  cmp rax, %d\n", value);
          printf("  je .Lcase%d.%d\n", switchLabel, value);
        } else { // defaultラベルの場合
          printf("  jmp .Ldefault%d\n", switchLabel);
        }
      }
      printf("  jmp .Lend%d\n", switchLabel);

      generate_statement(switchPattern->statement, labelCount, switchLabel,
                         switchLabel);

      printf(".Lend%d:\n", switchLabel);
      return;
    }
  }

  // match labeled
  {
    LabeledStatement *labeledPattern =
        statement_union_take_labeled(statementUnion);
    if (labeledPattern) {
      if (labeledPattern->constantExpression) // caseラベルの場合
      {
        Node *constantExpression = labeledPattern->constantExpression;
        int value;
        if (constantExpression->kind == NODE_NUM ||
            constantExpression->kind == NODE_CHAR)
          value = constantExpression->val;
        else if (constantExpression->kind == NODE_VAR &&
                 constantExpression->variable->kind == VARIABLE_GLOBAL &&
                 constantExpression->variable->initialization)
          value = constantExpression->variable->initialization->val;
        else
          ERROR_AT(constantExpression->source, "定数式ではありません");
        printf(".Lcase%d.%d:\n", latestSwitch, value);
      } else // defaultラベルの場合
        printf(".Ldefault%d:\n", latestSwitch);

      generate_statement(labeledPattern->statement, labelCount,
                         latestBreakTarget, latestSwitch);
      return;
    }
  }

  // match while
  {
    WhileStatement *whilePattern = statement_union_take_while(statementUnion);
    if (whilePattern) {
      int loopLabel = *labelCount;
      *labelCount += 1;

      printf(".Lbeginloop%d:\n", loopLabel);

      generate_expression(whilePattern->condition, labelCount);
      generate_value_extension(whilePattern->condition);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lend%d\n", loopLabel);

      generate_statement(whilePattern->statement, labelCount, loopLabel,
                         latestSwitch);

      printf(".Lcontinueloop%d:\n", loopLabel);
      printf("  jmp .Lbeginloop%d\n", loopLabel);

      printf(".Lend%d:\n", loopLabel);
      return;
    }
  }

  // match do-while
  {
    DoWhileStatement *doWhilePattern =
        statement_union_take_do_while(statementUnion);
    if (doWhilePattern) {
      int loopLabel = *labelCount;
      *labelCount += 1;

      printf(".Lbeginloop%d:\n", loopLabel);

      generate_statement(doWhilePattern->statement, labelCount, loopLabel,
                         latestSwitch);

      printf(".Lcontinueloop%d:\n", loopLabel);
      generate_expression(doWhilePattern->condition, labelCount);
      generate_value_extension(doWhilePattern->condition);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lend%d\n", loopLabel);

      printf("  jmp .Lbeginloop%d\n", loopLabel);

      printf(".Lend%d:\n", loopLabel);
      return;
    }
  }

  // match for
  {
    ForStatement *forPattern = statement_union_take_for(statementUnion);
    if (forPattern) {
      int loopLabel = *labelCount;
      *labelCount += 1;

      if (forPattern->initialization) {
        generate_expression(forPattern->initialization, labelCount);
        printf("  pop rax\n");
      }

      printf(".Lbeginloop%d:\n", loopLabel);

      if (forPattern->condition) {
        generate_expression(forPattern->condition, labelCount);
        generate_value_extension(forPattern->condition);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .Lend%d\n", loopLabel);
      }

      generate_statement(forPattern->statement, labelCount, loopLabel,
                         latestSwitch);

      printf(".Lcontinueloop%d:\n", loopLabel);
      if (forPattern->afterthought) {
        generate_expression(forPattern->afterthought, labelCount);
        printf("  pop rax\n");
      }
      printf("  jmp .Lbeginloop%d\n", loopLabel);

      printf(".Lend%d:\n", loopLabel);
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
        generate_statement(node->body, labelCount, latestBreakTarget,
                           latestSwitch);
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
      if (returnPattern->node)
        generate_expression(returnPattern->node, labelCount);
      else
        printf("  push 0\n"); //戻り値がvoid型のときはダミーの0をプッシュ
      printf("  pop rax\n");
      printf("  mov rsp, rbp\n");
      printf("  pop rbp\n");
      printf("  ret\n");
      return;
    }
  }

  // match break
  {
    BreakStatement *breakPattern = statement_union_take_break(statementUnion);
    if (breakPattern) {
      printf("  jmp .Lend%d    ", latestBreakTarget);
      INSERT_COMMENT("break statement");
      return;
    }
  }

  // match continue
  {
    ContinueStatement *continuePattern =
        statement_union_take_continue(statementUnion);
    if (continuePattern) {
      printf("  jmp .Lcontinueloop%d    ", latestBreakTarget);
      INSERT_COMMENT("continue statement");
      return;
    }
  }

  // match null
  {
    NullStatement *nullPattern = statement_union_take_null(statementUnion);
    if (nullPattern) {
      INSERT_COMMENT("null statement");
      return;
    }
  }

  // match expression
  {
    ExpressionStatement *expressionPattern =
        statement_union_take_expression(statementUnion);
    if (expressionPattern) {
      generate_expression(expressionPattern->node, labelCount);

      //文の評価結果をスタックからポップしてraxに格納
      //スタック溢れ対策も兼ねている
      printf("  pop rax\n");
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
  //定義された変数の分の領域を確保
  INSERT_COMMENT("function prologue start : %s", functionName);
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, %zu\n", functionDefinition->stackSize);
  INSERT_COMMENT("function prologue end : %s", functionName);

  //引数の代入処理
  INSERT_COMMENT("function arguments assign start : %s", functionName);
  int argumentStackOffset = 0;
  for (int i = vector_length(functionDefinition->arguments) - 1; i >= 0; i--) {
    Node *node = vector_get(functionDefinition->arguments, i);
    generate_variable(node);
    printf("  pop rax\n");

    if (i < 6) {
      printf("  mov [rax], %s\n", argumentRegister[i]);
    } else {
      // rbp+16 is memory argument eightbyte 0
      // rbp+16+8n is memory argument eightbyte n
      printf("  mov r11, [rbp+%d]\n", 16 + argumentStackOffset);
      argumentStackOffset += type_to_stack_size(node->type);
      printf("  mov [rax], r11\n");
    }
  }
  INSERT_COMMENT("function arguments assign end : %s", functionName);

  INSERT_COMMENT("function body start : %s", functionName);
  ListNode *statementList = functionDefinition->body->statementHead;
  while (statementList) {
    INSERT_COMMENT("statement start");

    //抽象構文木を降りながらコード生成
    // loopNestを無効な値にする(ループ外でbreakしないように)
    generate_statement(statementList->body, labelCount, -1, -1);
    statementList = statementList->next;

    INSERT_COMMENT("statement end");
  }
  INSERT_COMMENT("function body end : %s", functionName);

  //エピローグ
  //最後の式の評価結果はraxに格納済なので、それが戻り値となる
  INSERT_COMMENT("function epilogue start : %s", functionName);
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  INSERT_COMMENT("function epilogue end : %s", functionName);
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
  for (int i = 0; i < vector_length(program->functionDefinitions); i++) {
    const FunctionDefinition *function =
        vector_get(program->functionDefinitions, i);
    generate_function_definition(function, &labelCount);
  }
}
