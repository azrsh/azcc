#include "statement.h"
#include <stdlib.h>

// Statement Structure
typedef struct StatementUnion StatementUnion;

typedef enum {
  STATEMENT_NULL,
  STATEMENT_EXPRESSION,
  STATEMENT_IF,
  STATEMENT_SWITCH,
  STATEMENT_LABELED,
  STATEMENT_WHILE,
  STATEMENT_FOR,
  STATEMENT_COMPOUND,
  STATEMENT_RETURN,
  STATEMENT_BREAK,
  STATEMENT_CONTINUE,
} StatementKind;

struct StatementUnion {
  StatementKind tag;
  union {
    NullStatement *nullStatement;
    ExpressionStatement *expressionStatement;
    ReturnStatement *returnStatement;
    IfStatement *ifStatement;
    SwitchStatement *switchStatement;
    LabeledStatement *labeledStatement;
    WhileStatement *whileStatement;
    ForStatement *forStatement;
    CompoundStatement *compoundStatement;
    BreakStatement *breakStatement;
    ContinueStatement *continueStatement;
  };
};

StatementUnion *new_statement_union_null(NullStatement *statement) {
  StatementUnion *result = calloc(1, sizeof(StatementUnion));
  result->nullStatement = statement;
  result->tag = STATEMENT_NULL;
  return result;
}

StatementUnion *new_statement_union_expression(ExpressionStatement *statement) {
  StatementUnion *result = calloc(1, sizeof(StatementUnion));
  result->expressionStatement = statement;
  result->tag = STATEMENT_EXPRESSION;
  return result;
}

StatementUnion *new_statement_union_return(ReturnStatement *statement) {
  StatementUnion *result = calloc(1, sizeof(StatementUnion));
  result->returnStatement = statement;
  result->tag = STATEMENT_RETURN;
  return result;
}

StatementUnion *new_statement_union_if(IfStatement *statement) {
  StatementUnion *result = calloc(1, sizeof(StatementUnion));
  result->ifStatement = statement;
  result->tag = STATEMENT_IF;
  return result;
}

StatementUnion *new_statement_union_switch(SwitchStatement *statement) {
  StatementUnion *result = calloc(1, sizeof(StatementUnion));
  result->switchStatement = statement;
  result->tag = STATEMENT_SWITCH;
  return result;
}

StatementUnion *new_statement_union_labeled(LabeledStatement *statement) {
  StatementUnion *result = calloc(1, sizeof(StatementUnion));
  result->labeledStatement = statement;
  result->tag = STATEMENT_LABELED;
  return result;
}

StatementUnion *new_statement_union_while(WhileStatement *statement) {
  StatementUnion *result = calloc(1, sizeof(StatementUnion));
  result->whileStatement = statement;
  result->tag = STATEMENT_WHILE;
  return result;
}

StatementUnion *new_statement_union_for(ForStatement *statement) {
  StatementUnion *result = calloc(1, sizeof(StatementUnion));
  result->forStatement = statement;
  result->tag = STATEMENT_FOR;
  return result;
}

StatementUnion *new_statement_union_compound(CompoundStatement *statement) {
  StatementUnion *result = calloc(1, sizeof(StatementUnion));
  result->compoundStatement = statement;
  result->tag = STATEMENT_COMPOUND;
  return result;
}

StatementUnion *new_statement_union_break(BreakStatement *statement) {
  StatementUnion *result = calloc(1, sizeof(StatementUnion));
  result->breakStatement = statement;
  result->tag = STATEMENT_BREAK;
  return result;
}

StatementUnion *new_statement_union_continue(ContinueStatement *statement) {
  StatementUnion *result = calloc(1, sizeof(StatementUnion));
  result->continueStatement = statement;
  result->tag = STATEMENT_CONTINUE;
  return result;
}

NullStatement *statement_union_take_null(StatementUnion *statementUnion) {
  if (statementUnion->tag == STATEMENT_NULL)
    return statementUnion->nullStatement;
  return NULL;
}

ExpressionStatement *
statement_union_take_expression(StatementUnion *statementUnion) {
  if (statementUnion->tag == STATEMENT_EXPRESSION)
    return statementUnion->expressionStatement;
  return NULL;
}

ReturnStatement *statement_union_take_return(StatementUnion *statementUnion) {
  if (statementUnion->tag == STATEMENT_RETURN)
    return statementUnion->returnStatement;
  return NULL;
}

IfStatement *statement_union_take_if(StatementUnion *statementUnion) {
  if (statementUnion->tag == STATEMENT_IF)
    return statementUnion->ifStatement;
  return NULL;
}

SwitchStatement *statement_union_take_switch(StatementUnion *statementUnion) {
  if (statementUnion->tag == STATEMENT_SWITCH)
    return statementUnion->switchStatement;
  return NULL;
}

LabeledStatement *statement_union_take_labeled(StatementUnion *statementUnion) {
  if (statementUnion->tag == STATEMENT_LABELED)
    return statementUnion->labeledStatement;
  return NULL;
}

WhileStatement *statement_union_take_while(StatementUnion *statementUnion) {
  if (statementUnion->tag == STATEMENT_WHILE)
    return statementUnion->whileStatement;
  return NULL;
}

ForStatement *statement_union_take_for(StatementUnion *statementUnion) {
  if (statementUnion->tag == STATEMENT_FOR)
    return statementUnion->forStatement;
  return NULL;
}

CompoundStatement *
statement_union_take_compound(StatementUnion *statementUnion) {
  if (statementUnion->tag == STATEMENT_COMPOUND)
    return statementUnion->compoundStatement;
  return NULL;
}

BreakStatement *statement_union_take_break(StatementUnion *statementUnion) {
  if (statementUnion->tag == STATEMENT_BREAK)
    return statementUnion->breakStatement;
  return NULL;
}

ContinueStatement *
statement_union_take_continue(StatementUnion *statementUnion) {
  if (statementUnion->tag == STATEMENT_CONTINUE)
    return statementUnion->continueStatement;
  return NULL;
}
