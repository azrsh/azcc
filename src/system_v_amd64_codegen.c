#include "system_v_amd64_codegen.h"
#include "container.h"
#include "declaration.h"
#include "node.h"
#include "parse.h"
#include "returnstack.h"
#include "statement.h"
#include "type.h"
#include "util.h"
#include "variable.h"
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *argumentRegister64[6] = {"rdi", "rsi", "rdx",
                                            "rcx", "r8",  "r9"};
// static const char *argumentRegister32[6] = {"edi", "esi", "edx",
//                                            "ecx", "r8d", "r9d"};
// static const char *argumentRegister16[6] = {"di", "si",  "dx",
//                                            "cx", "r8w", "r9w"};
// static const char *argumentRegister8[6] = {"dil", "sil", "dl",
//                                           "cl",  "r8b", "r9b"};

static void generate_expression(Node *node, int *labelCount);
static void generate_variable(Node *node);
static void generate_function_call(Node *node, int *labelCount);
static void generate_assign_lhs(Node *node, int *labelCount);
static void generate_value_extension(Node *node);
static void generate_rhs_extension(Node *node);

static void generate_variable(Node *node) {
  assert(node->kind == NODE_VAR);

  const Variable *variable = node->variable;
  const char *name = string_to_char(variable->name);
  switch (variable->kind) {
  case VARIABLE_LOCAL:
    if (variable->storage == STORAGE_STATIC) {
      printf("  lea rax, %s.%d[rip]\n", name, variable->id);
      printf("  push rax\n");
      return;
    } else {
      printf("  lea rax, [rbp-%d]\n", variable->offset);
      printf("  push rax\n");
      return;
    }
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

static void generate_lhs_dot_operator(Node *node, int *labelCount) {
  assert(node->kind == NODE_DOT);

  INSERT_COMMENT("dot lhs operator start");
  assert(node->lhs->type->kind == TYPE_STRUCT ||
         node->lhs->type->kind == TYPE_UNION);
  assert(node->rhs->kind == NODE_VAR);

  //複雑な型(構造体、共用体)は常に左辺値扱いできる
  generate_expression(node->lhs, labelCount);
  printf("  pop rax\n");
  printf("  add rax, %d\n", node->rhs->variable->offset);
  printf("  push rax\n");
  INSERT_COMMENT("dot lhs operator end");
}

static void generate_assign_lhs(Node *node, int *labelCount) {
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
    generate_lhs_dot_operator(node, labelCount);
    return;
  }

  ERROR_AT(node->source, "代入の左辺として予期しないノードが指定されました");
}

static void generate_function_call(Node *node, int *labelCount) {
  if (node->kind != NODE_FUNC)
    ERROR("関数ではありません");

  const char *functionName = "mock";
  const int stackUnitSize = 8;
  const int alignmentSize = stackUnitSize * 2;

  INSERT_COMMENT("function call start : %s", functionName);

  Vector *arguments = node->functionCall->arguments;
  const int currentLabel = *labelCount;
  *labelCount += 1;

  //アライメント
  //スタックの使用予測に基づいてアライメント
  printf("  push 0\n");
  printf("  mov rax, rsp\n");

  //引数うちスタックに積まれる部分のサイズ計算
  int stackArgumentSize = 0;
  {
    int argumentStackLengthSum = 0;

    //戻り値がMEMORYクラスであるとき、第一引数として暗黙のうちに戻り値を格納するためのポインタが渡される
    Type *returnType = node->type;
    if (type_to_stack_size(returnType) > 2 * stackUnitSize)
      argumentStackLengthSum++;

    for (int i = 0; i < vector_length(arguments); i++) {
      Node *node = vector_get(arguments, i);
      int stackLength = type_to_stack_size(node->type) / stackUnitSize;
      argumentStackLengthSum += stackLength;
      if (stackLength > 2 || argumentStackLengthSum > 6)
        stackArgumentSize += stackLength * stackUnitSize;
    }
  }

  if (stackArgumentSize) {
    printf("  sub rax, %d\n", stackArgumentSize);
  }
  printf("  and rax, %d\n", alignmentSize - 1);
  printf("  jz .Lcall%d\n", currentLabel);
  printf("  push 1\n");
  printf(".Lcall%d:\n", currentLabel);

  //引数の評価
  {
    INSERT_COMMENT("function %s argument evaluation start", functionName);
    for (int i = vector_length(arguments) - 1; i >= 0; i--) {
      INSERT_COMMENT("function %s argument %d start", functionName, i);
      Node *argument = vector_get(arguments, i);
      generate_expression(argument, labelCount);

      // 複雑な型(構造体、共用体)の値はポインタとして格納されているので
      // 値に変換してスタックに積み直す
      if (argument->type->kind == TYPE_STRUCT ||
          argument->type->kind == TYPE_UNION) {
        int stackLength = type_to_stack_size(argument->type) / stackUnitSize;
        printf("  pop %s\n", "rax");
        for (int j = stackLength - 1; j >= 0; j--) {
          printf("  push [%s+%d]\n", "rax", j * 8);
        }
      }

      INSERT_COMMENT("function %s argument %d end", functionName, i);
    }
    INSERT_COMMENT("function %s argument evaluation end", functionName);
  }

  //呼び出し先アドレスの計算
  {
    // if (string_compare(&node->functionCall->name, char_to_string("calloc"))
    // ||
    //    string_compare(&node->functionCall->name, char_to_string("strlen")) ||
    //    string_compare(&node->functionCall->name, char_to_string("memcmp")) ||
    //    string_compare(&node->functionCall->name, char_to_string("memcpy")))
    //  printf("  call %s@PLT\n", functionName);
    // else

    generate_expression(node->lhs, labelCount);
    printf("  pop r10\n");
  }

  //引数の評価中に関数の呼び出しが発生してレジスタが破壊される可能性があるので
  //引数を全て評価してからレジスタに割り当て
  // r10レジスタを呼び出し先アドレスの保存に使用しているため、変更してはいけない
  {
    INSERT_COMMENT("function %s argument register allocation start",
                   functionName);
    int registerIndex = 0;

    //戻り値がMEMORYクラスであるとき、第一引数として暗黙のうちに戻り値を格納するためのポインタが渡される
    Type *returnType = node->type;
    if (type_to_stack_size(returnType) > 2 * stackUnitSize) {
      INSERT_COMMENT("function %s implicit argument assign start",
                     functionName);
      printf("  lea %s, [rbp-%d]\n", "rax",
             node->functionCall->returnStack->offset);
      printf("  mov %s, %s\n", argumentRegister64[registerIndex++], "rax");
      INSERT_COMMENT("function %s implicit argument assign end", functionName);
    } else {
      INSERT_COMMENT("function %s no implicit argument insertion",
                     functionName);
    }

    int currentStackIndex = 0;
    for (int i = 0; i < vector_length(arguments); i++) {
      Node *node = vector_get(arguments, i);
      const int stackLength = type_to_stack_size(node->type) / stackUnitSize;
      if (registerIndex + stackLength > 6)
        break;

      if (stackLength > 2) {
        currentStackIndex += stackLength;
        continue;
      }

      for (int j = 0; j < stackLength; j++)
        printf("  mov %s, [rsp+%d]\n", argumentRegister64[registerIndex++],
               (currentStackIndex + j) * 8);

      //もしレジスタに移した値よりも前に引数があれば詰める
      for (int j = currentStackIndex - 1; j >= 0; j--) {
        printf("  mov r11, [rsp+%d]\n", j * stackUnitSize);
        printf("  mov [rsp+%d], r11\n", (j + stackLength) * stackUnitSize);
      }

      printf("  add rsp, %d\n", stackLength * stackUnitSize);
    }

    INSERT_COMMENT("function %s argument register allocation end",
                   functionName);
  }

  printf("  mov rax, 0\n");
  printf("  call r10\n");

  //スタックに積んだ引数を処理
  if (stackArgumentSize > 0) {
    printf("  add rsp, %d\n", stackArgumentSize);
  }

  //アライメントの判定とスタックの復元
  printf("  pop r11\n");
  printf("  cmp r11, 0\n");
  printf("  je .Lend%d\n", currentLabel);
  printf("  add rsp, %d\n", stackUnitSize);
  printf(".Lend%d:\n", currentLabel);

  if (node->type->kind == TYPE_STRUCT || node->type->kind == TYPE_UNION) {
    /*
     * stack_length > 1 && stack_length <= 2 <=> stack_length == 2のとき
     * raxとrdxに戻り値が値として格納されているのでポインタに変換
     *
     * stack_length == 1のとき
     * raxに戻り値が値として格納されているのでポインタに変換
     +
     * stack_length > 2のとき
     * 値へのポインタが返ってくるので何もしなくてよい
     */
    Type *returnType = node->type;
    Variable *returnStack = node->functionCall->returnStack;
    if (type_to_stack_size(returnType) / stackUnitSize == 1) {
      printf("  mov [rbp-%d], rax\n", returnStack->offset);
      printf("  lea rax, [rbp-%d]\n", returnStack->offset);
    } else if (type_to_stack_size(returnType) / stackUnitSize == 2) {
      printf("  mov [rbp-%d], rax\n", returnStack->offset);
      printf("  mov [rbp-%d], rdx\n", returnStack->offset - stackUnitSize);
      printf("  lea rax, [rbp-%d]\n", returnStack->offset);
    }
  }

  printf("  push rax\n");

  //内部で扱う値は基本的に64bit整数なので拡張
  if (node->type->kind != TYPE_VOID)
    generate_value_extension(node);

  INSERT_COMMENT("function call end : %s", functionName);
}

static void generate_cast(Node *node) {
  assert(node->kind == NODE_CAST);

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
    // assert(0); // forbidden
  }

  // void*とポインタ型のキャストを許可
  // 比較演算子におけるポインタ型と0の比較を許可

  printf("  push rax\n");

  INSERT_COMMENT("cast end");
}

static void generate_assign_i64(const char *destination, const char *source) {
  printf("  mov [%s], %s\n", destination, source);
}

static void generate_assign_i32(const char *destination, const char *source) {
  printf("  mov DWORD PTR [%s], %s\n", destination, source);
}

static void generate_assign_i8(const char *destination, const char *source) {
  printf("  mov BYTE PTR [%s], %s\n", destination, source);
}

static void generate_assign_complex(int size, const char *destination,
                                    const char *source) {
  int current = 0;
  for (int i = 0; i < size / 8; i++) {
    printf("  mov r11, [%s+%d]\n", source, current);
    printf("  mov [%s+%d], r11\n", destination, current);
    current += 8;
  }
  if (size % 8 >= 4) {
    printf("  mov r11d, DWORD PTR [%s+%d]\n", source, current);
    printf("  mov DWORD PTR [%s+%d], r11d\n", destination, current);
    current += 4;
  }
  if (size % 4 >= 2) {
    printf("  mov r11w, WORD PTR [%s+%d]\n", source, current);
    printf("  mov WORD PTR [%s+%d], r11w\n", destination, current);
    current += 2;
  }
  if (size % 2 >= 1) {
    printf("  mov r11b, BYTE PTR [%s+%d]\n", source, current);
    printf("  mov BYTE PTR [%s+%d], r11b\n", destination, current);
    current += 1;
  }
}

static void generate_rhs_extension(Node *node) {
  //暗黙的なキャスト
  //配列はポインタに
  //それ以外の値は64bitに符号拡張
  printf("  pop rax\n");
  switch (node->type->kind) {
  case TYPE_CHAR:
  case TYPE_BOOL:
    printf("  movsx rax, BYTE PTR [rax]\n");
    break;
  case TYPE_INT:
  case TYPE_ENUM:
    printf("  movsx rax, DWORD PTR [rax]\n");
    break;
  case TYPE_PTR:
    printf("  mov rax, [rax]\n");
    break;
  case TYPE_STRUCT:
  case TYPE_UNION:
  case TYPE_ARRAY:
  case TYPE_FUNC:
    break; //配列と関数はポインタのままにする
  case TYPE_VOID:
    ERROR_AT(node->source, "許可されていない型の値です");
  }
  printf("  push rax\n");
}

static void generate_value_extension(Node *node) {
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
  case TYPE_STRUCT: //構造体はそのままでよい
  case TYPE_UNION:  //共用体はそのままでよい
  case TYPE_FUNC:   //関数はそのままでよい
    break;
  case TYPE_VOID:
    ERROR_AT(node->source, "許可されていない型の値です");
  }
  printf("  push rax\n");
}

static void generate_expression(Node *node, int *labelCount) {
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
  case NODE_BNOT:
    INSERT_COMMENT("bitwise not start");
    generate_expression(node->lhs, labelCount);
    printf("  pop rax\n");
    printf("  not rax\n");
    printf("  push rax\n");
    INSERT_COMMENT("bitwise not end");
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
    //構造体は常に左辺値にできる
    generate_lhs_dot_operator(node, labelCount);
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

    printf("  pop rdi\n");
    printf("  pop rax\n");
    if (node->lhs->type->kind == TYPE_STRUCT ||
        node->type->kind == TYPE_UNION) {
      generate_assign_complex(lhsSize, "rax", "rdi");
    } else {
      if (lhsSize == 1 && rhsSize == 1)
        generate_assign_i8("rax", "dil");
      else if (lhsSize == 4 && rhsSize == 4)
        generate_assign_i32("rax", "edi");
      else if (lhsSize == 8 && rhsSize == 8)
        generate_assign_i64("rax", "rdi");
      else
        ERROR_AT(node->source, "予期しない代入");
    }
    printf("  push rdi\n");

    INSERT_COMMENT("assign end");
    return;
  case NODE_CAST:
    generate_expression(node->lhs, labelCount);
    generate_cast(node);
    return;
  case NODE_LAND: {
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
  }
  case NODE_LOR: {
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
  }
  case NODE_COND: {
    INSERT_COMMENT("conditional start");
    int condLabel = *labelCount;
    *labelCount += 1;
    generate_expression(node->condition, labelCount);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lsecondcond%d\n", condLabel);
    generate_expression(node->lhs, labelCount);
    printf("  jmp .Lendcond%d\n", condLabel);
    printf(".Lsecondcond%d:\n", condLabel);
    generate_expression(node->rhs, labelCount);
    printf(".Lendcond%d:\n", condLabel);
    INSERT_COMMENT("conditional end");
    return;
  }
  case NODE_COMMA:
    generate_expression(node->lhs, labelCount);
    printf("  pop rax\n");
    generate_expression(node->rhs, labelCount);
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
  case NODE_BAND:
  case NODE_BXOR:
  case NODE_BOR:
  case NODE_LSHIFT:
  case NODE_RSHIFT:
    break; //次のswitch文で判定する
  }

  generate_expression(node->lhs, labelCount);
  generate_expression(node->rhs, labelCount);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case NODE_ADD: {
    INSERT_COMMENT("start add node");

    // int+int、pointer+int、int+pointerのみを許可する
    Type *lhsBase = node->lhs->type->base;
    Type *rhsBase = node->rhs->type->base;
    if (lhsBase && !rhsBase)
      printf("  imul rdi, %d\n", type_to_size(lhsBase));
    if (!lhsBase && rhsBase)
      printf("  imul rax, %d\n", type_to_size(rhsBase));

    printf("  add rax, rdi\n");
    INSERT_COMMENT("end add node");
    break;
  }
  case NODE_SUB: {
    INSERT_COMMENT("start sub node");

    // int-int、pointer-int、pointer-pointerのみを許可する
    Type *lhsBase = node->lhs->type->base;
    Type *rhsBase = node->rhs->type->base;
    if (lhsBase && !rhsBase)
      printf("  imul rdi, %d\n", type_to_size(lhsBase));

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
  case NODE_BAND:
    printf("  and rax, rdi\n");
    break;
  case NODE_BXOR:
    printf("  xor rax, rdi\n");
    break;
  case NODE_BOR:
    printf("  or rax, rdi\n");
    break;
  case NODE_LSHIFT:
    printf("  mov rcx, rdi\n");
    printf("  shl rax, cl\n"); //論理シフトのみ
    break;
  case NODE_RSHIFT:
    printf("  mov rcx, rdi\n");
    printf("  shr rax, cl\n"); //論理シフトのみ
    break;
  case NODE_LAND:
  case NODE_LOR:
  case NODE_LNOT:
  case NODE_BNOT:
  case NODE_COND:
  case NODE_COMMA:
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

static void generate_string_literal(int index, const char *string) {
  printf("  .data\n");
  printf(".LC%d:\n", index);
  printf("  .string \"%s\"\n", string);
}

static void generate_global_variable_initializer(Type *type,
                                                 Node *initializer) {
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

static void generate_static_memory_variable(const Variable *variable) {
  assert(variable->kind == VARIABLE_GLOBAL ||
         variable->storage == STORAGE_STATIC);

  const char *name = string_to_char(variable->name);
  if (variable->storage != STORAGE_STATIC)
    printf("  .globl %s\n", name);

  printf("  .data\n");

  if (variable->kind == VARIABLE_GLOBAL)
    printf("%s:\n", name);
  else
    printf("%s.%d:\n", name, variable->id);
  generate_global_variable_initializer(variable->type,
                                       variable->initialization);
}

static void generate_statement(StatementUnion *statementUnion, int *labelCount,
                               int returnTarget, int latestBreakTarget,
                               int latestSwitch) {
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

      generate_statement(ifPattern->thenStatement, labelCount, returnTarget,
                         latestBreakTarget, latestSwitch);

      if (ifPattern->elseStatement) {
        printf("  jmp .Lendif%d\n", ifLabel);
        printf(".Lelse%d:\n", ifLabel);
        generate_statement(ifPattern->elseStatement, labelCount, returnTarget,
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

      generate_statement(switchPattern->statement, labelCount, returnTarget,
                         switchLabel, switchLabel);

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
      } else if (labeledPattern->name)

        printf(".L%s:\n", string_to_char(labeledPattern->name));
      else // defaultラベルの場合
        printf(".Ldefault%d:\n", latestSwitch);

      generate_statement(labeledPattern->statement, labelCount, returnTarget,
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

      generate_statement(whilePattern->statement, labelCount, returnTarget,
                         loopLabel, latestSwitch);

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

      generate_statement(doWhilePattern->statement, labelCount, returnTarget,
                         loopLabel, latestSwitch);

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

      generate_statement(forPattern->statement, labelCount, returnTarget,
                         loopLabel, latestSwitch);

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
      for (int i = 0; i < vector_length(compoundPattern->blockItemList); i++) {
        BlockItem *item = vector_get(compoundPattern->blockItemList, i);
        if (item->declaration) {
          for (int j = 0; j < vector_length(item->declaration->declarators);
               j++) {
            Node *declarator = vector_get(item->declaration->declarators, j);
            generate_expression(declarator, labelCount);
            printf("  pop rax\n");
          }
        } else {
          generate_statement(item->statement, labelCount, returnTarget,
                             latestBreakTarget, latestSwitch);
        }
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

      printf("  jmp .Lreturn%d\n", returnTarget);
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

  // match goto
  {
    GotoStatement *gotoPattern = statement_union_take_goto(statementUnion);
    if (gotoPattern) {
      INSERT_COMMENT("goto statement");
      printf("  jmp .L%s\n", string_to_char(gotoPattern->label));
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
static void generate_function_definition(Variable *variable, int *labelCount) {
  const FunctionDefinition *definition = variable->function;
  const char *functionName = string_to_char(definition->name);
  const int returnTarget = *labelCount;
  *labelCount += 1;

  //ラベルを生成
  printf("  .text\n");
  if (variable->storage != STORAGE_STATIC)
    printf("  .global %s\n", functionName);
  // macは先頭に_を挿入するらしい
  printf("%s:\n", functionName);

  //プロローグ
  //定義された変数の分の領域を確保
  {
    INSERT_COMMENT("function prologue start : %s", functionName);
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    {
      //構造体の値渡しの戻り値のための暗黙の第一引数が存在するなら
      //それを保管するためのスタック領域を確保
      size_t stackSize = definition->stackSize;
      if (type_to_stack_size(definition->returnType) > 2)
        stackSize += 8;
      printf("  sub rsp, %zu\n", stackSize);
    }
    INSERT_COMMENT("function prologue end : %s", functionName);
  }

  //引数の代入処理
  {
    INSERT_COMMENT("function arguments assign start : %s", functionName);

    int registerIndex = 0;

    //構造体の値渡しの戻り値のための暗黙の第一引数の処理
    if (type_to_stack_size(definition->returnType) > 2 * 8) {
      printf("  mov [rbp-%zu], %s\n", definition->stackSize + 8,
             argumentRegister64[registerIndex++]);
    }

    int argumentStackOffset = 0;
    for (int i = 0; i < vector_length(definition->arguments); i++) {
      Node *node = vector_get(definition->arguments, i);
      generate_variable(node);
      printf("  pop rax\n");

      //このコンパイラにはINTEGERクラスしか存在しないのでこれでよい
      // INTEGERクラスとMEMORYクラスの境界は16byte(2 eight bytes)
      const int stackLength = type_to_stack_size(node->type) / 8;
      if (stackLength <= 2 && registerIndex + stackLength <= 6) {
        for (int j = 0; j < stackLength; j++) {
          printf("  mov [%s+%d], %s\n", "rax", j * 8,
                 argumentRegister64[registerIndex++]);
        }
      } else {
        // rbp+16 is memory argument eightbyte 0
        // rbp+16+8n is memory argument eightbyte n
        for (int j = 0; j < stackLength; j++) {
          printf("  mov %s, [rbp+%d]\n", "r11",
                 16 + argumentStackOffset + j * 8);
          printf("  mov [%s+%d], %s\n", "rax", j * 8, "r11");
        }
        argumentStackOffset += type_to_stack_size(node->type);
      }
    }
    INSERT_COMMENT("function arguments assign end : %s", functionName);
  }

  INSERT_COMMENT("function body start : %s", functionName);
  generate_statement(new_statement_union_compound(definition->body), labelCount,
                     returnTarget, -1, -1);
  INSERT_COMMENT("function body end : %s", functionName);

  //エピローグ
  //最後の式の評価結果はraxに格納済なので、それが戻り値となる
  {
    INSERT_COMMENT("function epilogue start : %s", functionName);
    printf(".Lreturn%d:\n", returnTarget);
    if (definition->returnType->kind == TYPE_STRUCT ||
        definition->returnType->kind == TYPE_UNION) {
      Type *returnType = definition->returnType;
      int stackLength = type_to_stack_size(returnType) / 8;
      /*
       * stack_size <= 1 <=> stack_size == 1のとき
       * ポインタになっている戻り値から値を取り出す
       *
       * stack_size > 1 && stack_size <= 2 <=> stack_size == 2のとき
       * ポインタになっている戻り値から値を取り出し
       * 下位64bitをrdx(2nd return register)に代入
       *
       * stackLength > 2のとき
       * 暗黙の第一引数として渡されたポインタの先のスタック領域に戻り値を格納
       */
      if (stackLength == 1) {
        printf("  mov rax, [rax]\n");
      } else if (stackLength == 2) {
        printf("  mov rdx, [rax+8]\n");
        printf("  mov rax, [rax]\n");
      } else if (stackLength > 2) {
        printf("  mov rdx, [rbp-%zu]\n", definition->stackSize + 8);
        generate_assign_complex(type_to_size(returnType), "rdx", "rax");
        printf("  mov rax, [rbp-%zu]\n", definition->stackSize + 8);
      }
    }

    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    INSERT_COMMENT("function epilogue end : %s", functionName);
  }
}

void generate_code_system_v_amd64(Program *program) {
  //アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");

  for (int i = 0; i < vector_length(program->stringLiterals); i++) {
    const char *string = vector_get(program->stringLiterals, i);
    generate_string_literal(i, string);
  }

  for (int i = 0; i < vector_length(program->staticMemoryVariables); i++) {
    Variable *variable = vector_get(program->staticMemoryVariables, i);

    // 静的領域に保存される変数にidを割り当て
    // この処理は関数の生成前に行われなければならない
    // variableはポインタなので、コード生成に使用されるデータにも伝播される
    variable->id = i;

    generate_static_memory_variable(variable);
  }

  int labelCount = 0;
  for (int i = 0; i < vector_length(program->functionDefinitions); i++) {
    Variable *function = vector_get(program->functionDefinitions, i);
    allocate_return_stack_to_function(function->function);
    generate_function_definition(function, &labelCount);
  }
}
