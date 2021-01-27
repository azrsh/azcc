#include "returnstack.h"
#include "container.h"
#include "functioncall.h"
#include "node.h"
#include "parse.h"
#include "statement.h"
#include "type.h"
#include "util.h"
#include "variable.h"

typedef struct StackAllocContext StackAllocContext;
struct StackAllocContext {
  int currentStackOffset;
};

//式への型付け
void allocate_return_stack_to_node(Node *node, StackAllocContext *context) {
  if (!node) {
    ERROR("指定されたノードが存在しません");
  }

  switch (node->kind) {
  case NODE_NUM:
  case NODE_CHAR:
  case NODE_STRING:
  case NODE_VAR:
    return;
  case NODE_ARRAY: {
    for (int i = 0; i < vector_length(node->elements); i++) {
      Node *element = vector_get(node->elements, i);
      allocate_return_stack_to_node(element, context);
    }
    return;
  }
  case NODE_LNOT:
  case NODE_BNOT:
  case NODE_REF:
  case NODE_DEREF:
  case NODE_CAST:
    allocate_return_stack_to_node(node->lhs, context);
    return;
  case NODE_FUNC: {
    allocate_return_stack_to_node(node->lhs, context);

    if (type_to_stack_size(node->type) > 1 * 8) {
      //識別子として無効な変数名を生成
      const String *name =
          string_concat(char_to_string("0"), char_to_string("return stack"));
      // Variable *returnVariable =
      //    variable_to_local(new_variable(function->returnType, name));
      Variable *returnVariable = calloc(1, sizeof(Variable));
      returnVariable->kind = VARIABLE_LOCAL;
      returnVariable->name = name;
      returnVariable->type = node->type;
      context->currentStackOffset += type_to_stack_size(node->type);
      returnVariable->offset = context->currentStackOffset;
      node->functionCall->returnStack = returnVariable;
    }

    for (int i = 0; i < vector_length(node->functionCall->arguments); i++) {
      Node *arg = vector_get(node->functionCall->arguments, i);
      allocate_return_stack_to_node(arg, context);
    }
    return;
  }
  case NODE_ASSIGN:
  case NODE_DOT:
  case NODE_ADD:
  case NODE_SUB:
  case NODE_MUL:
  case NODE_DIV:
  case NODE_MOD:
  case NODE_EQ:
  case NODE_NE:
  case NODE_LT:
  case NODE_LE:
  case NODE_LAND:
  case NODE_LOR:
  case NODE_BAND:
  case NODE_BXOR:
  case NODE_BOR:
  case NODE_LSHIFT:
  case NODE_RSHIFT:
  case NODE_COND:
  case NODE_COMMA:
    allocate_return_stack_to_node(node->lhs, context);
    allocate_return_stack_to_node(node->rhs, context);
    return; //次のswitch文で判定する
  }

  ERROR_AT(node->source, "予期しないノードが指定されました");
}

void allocate_return_stack_to_declaration(Declaration *declaration,
                                          StackAllocContext *context) {
  ERROR("invalid abstract syntax tree");
}

void allocate_return_stack_to_statement(StatementUnion *statementUnion,
                                        StackAllocContext *context) {
  if (!statementUnion) {
    ERROR("指定された文が存在しません");
  }

  // match if
  {
    IfStatement *ifPattern = statement_union_take_if(statementUnion);
    if (ifPattern) {
      allocate_return_stack_to_node(ifPattern->condition, context);
      allocate_return_stack_to_statement(ifPattern->thenStatement, context);
      if (ifPattern->elseStatement)
        allocate_return_stack_to_statement(ifPattern->elseStatement, context);
      return;
    }
  }

  // match switch
  {
    SwitchStatement *switchPattern =
        statement_union_take_switch(statementUnion);
    if (switchPattern) {
      allocate_return_stack_to_node(switchPattern->condition, context);
      allocate_return_stack_to_statement(switchPattern->statement, context);
      return;
    }
  }

  // match labeled
  {
    LabeledStatement *labeledPattern =
        statement_union_take_labeled(statementUnion);
    if (labeledPattern) {
      if (labeledPattern->constantExpression)
        allocate_return_stack_to_node(labeledPattern->constantExpression,
                                      context);
      allocate_return_stack_to_statement(labeledPattern->statement, context);
      return;
    }
  }

  // match while
  {
    WhileStatement *whilePattern = statement_union_take_while(statementUnion);
    if (whilePattern) {
      allocate_return_stack_to_node(whilePattern->condition, context);
      allocate_return_stack_to_statement(whilePattern->statement, context);
      return;
    }
  }

  // match do-while
  {
    DoWhileStatement *doWhilePattern =
        statement_union_take_do_while(statementUnion);
    if (doWhilePattern) {
      allocate_return_stack_to_node(doWhilePattern->condition, context);
      allocate_return_stack_to_statement(doWhilePattern->statement, context);
      return;
    }
  }

  // match for
  {
    ForStatement *forPattern = statement_union_take_for(statementUnion);
    if (forPattern) {
      if (forPattern->initialization)
        allocate_return_stack_to_node(forPattern->initialization, context);
      if (forPattern->condition)
        allocate_return_stack_to_node(forPattern->condition, context);
      if (forPattern->afterthought)
        allocate_return_stack_to_node(forPattern->afterthought, context);
      allocate_return_stack_to_statement(forPattern->statement, context);
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
        if (item->declaration)
          allocate_return_stack_to_declaration(item->declaration, context);
        else if (item->statement)
          allocate_return_stack_to_statement(item->statement, context);
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
        allocate_return_stack_to_node(returnPattern->node, context);
      return;
    }
  }

  // match break
  {
    BreakStatement *breakPattern = statement_union_take_break(statementUnion);
    if (breakPattern) {
      return;
    }
  }

  // match continue
  {
    ContinueStatement *continuePattern =
        statement_union_take_continue(statementUnion);
    if (continuePattern) {
      return;
    }
  }

  // match null
  {
    NullStatement *nullPattern = statement_union_take_null(statementUnion);
    if (nullPattern) {
      return;
    }
  }

  // match goto
  {
    GotoStatement *gotoPattern = statement_union_take_goto(statementUnion);
    if (gotoPattern) {
      return;
    }
  }

  // match expression
  {
    ExpressionStatement *expressionPattern =
        statement_union_take_expression(statementUnion);
    if (expressionPattern) {
      allocate_return_stack_to_node(expressionPattern->node, context);
      return;
    }
  }

  ERROR("予期しない文が指定されました");
}

void allocate_return_stack_to_function(FunctionDefinition *function) {
  StackAllocContext context;
  context.currentStackOffset = function->stackSize;
  allocate_return_stack_to_statement(
      new_statement_union_compound(function->body), &context);
  function->stackSize = context.currentStackOffset;
}
