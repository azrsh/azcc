/*
 * 文をパースする。
 * statementのみを公開する。
 */

#include "analyze.h"
#include "container.h"
#include "declaration.h"
#include "declarationparse.h"
#include "expressionparse.h"
#include "node.h"
#include "parseutil.h"
#include "statement.h"
#include "tokenize.h"
#include "type.h"
#include "typecheck.h"
#include "util.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

// EBNF
// statement = null_statement | expression_statement | return_statement |
// if_statement | switch_statement | labeled_statement | while_statementa |
// do_while_statement | break_statement | continue_statement
// null_statement = ";"
// expression_statement = expression ";"
// return_statement = "return" expression ";"
// if_statement = "if" "(" expression ")" statement ("else" statement)?
// switch_statement = "switch" "(" expression ")" statement
// labeled_statement = (("case"? constant-expression) | "default") ":"
// statement while_statement = "while" "(" expression ")" statement
// do_while_statement = do statement "while" "(" expression ")" ";"
// for_statement = "for" "(" expression ";" expression ";" expression ")"
// statement
// compound_statement = "{" statement* "}"
// break_statement = "break" ";"
// continue_statement = "continue" ";"

StatementUnion *statement(ParseContext *context);

NullStatement *null_statement(void);
ExpressionStatement *expression_statement(ParseContext *context);
ReturnStatement *return_statement(ParseContext *context);
IfStatement *if_statement(ParseContext *context);
SwitchStatement *switch_statement(ParseContext *context);
LabeledStatement *labeled_statement(ParseContext *context);
WhileStatement *while_statement(ParseContext *context);
DoWhileStatement *do_while_statement(ParseContext *context);
ForStatement *for_statement(ParseContext *context);
CompoundStatement *compound_statement(ParseContext *context);
BreakStatement *break_statement(void);
ContinueStatement *continue_statement(void);
GotoStatement *goto_statement(void);

//文をパースする
StatementUnion *statement(ParseContext *context) {
  ReturnStatement *returnPattern = return_statement(context);
  if (returnPattern) {
    return new_statement_union_return(returnPattern);
  }

  IfStatement *ifPattern = if_statement(context);
  if (ifPattern) {
    return new_statement_union_if(ifPattern);
  }

  SwitchStatement *switchPattern = switch_statement(context);
  if (switchPattern) {
    return new_statement_union_switch(switchPattern);
  }

  LabeledStatement *labeledPattern = labeled_statement(context);
  if (labeledPattern) {
    return new_statement_union_labeled(labeledPattern);
  }

  WhileStatement *whilePattern = while_statement(context);
  if (whilePattern) {
    return new_statement_union_while(whilePattern);
  }

  DoWhileStatement *doWhilePattern = do_while_statement(context);
  if (doWhilePattern) {
    return new_statement_union_do_while(doWhilePattern);
  }

  ForStatement *forPattern = for_statement(context);
  if (forPattern) {
    return new_statement_union_for(forPattern);
  }

  CompoundStatement *compoundPattern = compound_statement(context);
  if (compoundPattern) {
    return new_statement_union_compound(compoundPattern);
  }

  BreakStatement *breakPattern = break_statement();
  if (breakPattern) {
    return new_statement_union_break(breakPattern);
  }

  ContinueStatement *continuePattern = continue_statement();
  if (continuePattern) {
    return new_statement_union_continue(continuePattern);
  }

  NullStatement *nullPattern = null_statement();
  if (nullPattern) {
    return new_statement_union_null(nullPattern);
  }

  GotoStatement *gotoPattern = goto_statement();
  if (gotoPattern) {
    return new_statement_union_goto(gotoPattern);
  }

  ExpressionStatement *expressionPattern = expression_statement(context);
  return new_statement_union_expression(expressionPattern);
}

NullStatement *null_statement(void) {
  if (!consume(";"))
    return NULL;

  return calloc(1, sizeof(NullStatement));
}

// 式の文をパースする
ExpressionStatement *expression_statement(ParseContext *context) {
  ExpressionStatement *result = calloc(1, sizeof(ExpressionStatement));
  result->node = expression(context);
  expect(";");
  return result;
}

// return文をパースする
ReturnStatement *return_statement(ParseContext *context) {
  if (!consume("return")) {
    return NULL;
  }

  ReturnStatement *result = calloc(1, sizeof(ReturnStatement));

  if (!consume(";")) {
    result->node = expression(context);
    expect(";");
  }
  return result;
}

// if文をパースする
IfStatement *if_statement(ParseContext *context) {
  if (!consume("if") || !consume("(")) {
    return NULL;
  }

  IfStatement *result = calloc(1, sizeof(IfStatement));
  result->condition = expression(context);

  expect(")");

  result->thenStatement = statement(context);
  if (consume("else")) {
    result->elseStatement = statement(context);
  }

  return result;
}

// switch文をパースする
SwitchStatement *switch_statement(ParseContext *context) {
  if (!consume("switch") || !consume("(")) {
    return NULL;
  }

  SwitchStatement *result = calloc(1, sizeof(SwitchStatement));
  result->condition = expression(context);
  result->labeledStatements = new_vector(16);

  expect(")");

  context->function->switchStatementNest =
      list_push_front(context->function->switchStatementNest, result);
  result->statement = statement(context);
  context->function->switchStatementNest =
      context->function->switchStatementNest->next;

  return result;
}

// label付の文をパースする
LabeledStatement *labeled_statement(ParseContext *context) {
  Token *tokenHead = token;

  LabeledStatement *result = calloc(1, sizeof(LabeledStatement));
  bool isCaseLabel = consume("case");
  if (isCaseLabel) {
    result->constantExpression = expression(context);
  }
  if (isCaseLabel || consume("default")) {
    expect(":");

    //最も新しいswitch文に自身を登録
    SwitchStatement *switchStatement =
        context->function->switchStatementNest->body;
    if (switchStatement)
      vector_push_back(switchStatement->labeledStatements, result);
    else
      ERROR_AT(tokenHead->string->head,
               "switch文の外でcaseまたはdefaultラベルが定義されました");

    result->statement = statement(context);
    return result;
  }

  Token *identifier = consume_identifier();
  if (identifier && consume(":")) {
    result->name = identifier->string;
    result->statement = statement(context);
    return result;
  }

  token = tokenHead;
  return NULL;
}

// while文をパースする
WhileStatement *while_statement(ParseContext *context) {
  if (!consume("while") || !consume("(")) {
    return NULL;
  }

  WhileStatement *result = calloc(1, sizeof(WhileStatement));
  result->condition = expression(context);

  expect(")");

  result->statement = statement(context);

  return result;
}

// do-while文をパースする
DoWhileStatement *do_while_statement(ParseContext *context) {
  Token *tokenHead = token;

  if (!consume("do"))
    return NULL;

  DoWhileStatement *result = calloc(1, sizeof(DoWhileStatement));
  result->statement = statement(context);

  if (!consume("while") || !consume("(")) {
    token = tokenHead;
    return NULL;
  }

  result->condition = expression(context);

  expect(")");
  expect(";");
  return result;
}

// for文をパースする
ForStatement *for_statement(ParseContext *context) {
  if (!consume("for") || !consume("(")) {
    return NULL;
  }

  //スコープの生成
  context = new_scope_context(context);

  ForStatement *result = calloc(1, sizeof(ForStatement));

  // initialization
  if (!consume(";")) {
    Declaration *initialDeclaration =
        declaration(declaration_specifier(context), context);
    if (initialDeclaration) {
      //結果がNULLならNULLが代入されてよい
      result->initialization =
          analyze_local_declaration(initialDeclaration, context);
    } else {
      result->initialization = expression(context);
      expect(";");
    }
  }

  // condition
  if (!consume(";")) {
    result->condition = expression(context);
    expect(";");
  }

  // afterthought
  if (!consume(")")) {
    result->afterthought = expression(context);
    expect(")");
  }

  result->statement = statement(context);

  return result;
}

// 複文をパースする
CompoundStatement *compound_statement(ParseContext *context) {
  if (!consume("{")) {
    return NULL;
  }

  CompoundStatement *result = calloc(1, sizeof(CompoundStatement));

  //新しいスコープなので先頭に新しい変数テーブルを追加
  context = new_scope_context(context);

  //ブロック内の文をパ-ス
  result->blockItemList = new_vector(32);
  while (!consume("}")) {
    BlockItem *blockItem = calloc(1, sizeof(BlockItem));
    blockItem->declaration =
        declaration(declaration_specifier(context), context);
    if (blockItem->declaration) {
      blockItem->statement =
          analyze_blockItem_declaration(blockItem->declaration, context);
      blockItem->declaration = NULL;
      if (!blockItem->statement)
        continue;
    } else {
      blockItem->statement = statement(context);
    }
    vector_push_back(result->blockItemList, blockItem);
  }

  return result;
}

// break文をパースする
BreakStatement *break_statement(void) {
  if (!consume("break")) {
    return NULL;
  }
  expect(";");
  BreakStatement *result = calloc(1, sizeof(BreakStatement));
  return result;
}

// continue文をパースする
ContinueStatement *continue_statement(void) {
  if (!consume("continue")) {
    return NULL;
  }
  expect(";");
  ContinueStatement *result = calloc(1, sizeof(ContinueStatement));
  return result;
}

GotoStatement *goto_statement(void) {
  if (!consume("goto")) {
    return NULL;
  }

  GotoStatement *result = calloc(1, sizeof(GotoStatement));
  result->label = expect_identifier()->string;
  expect(";");
  return result;
}
