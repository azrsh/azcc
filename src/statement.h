#ifndef STATEMENT_H
#define STATEMENT_H

#include "container.h"
#include "declaration.h"
#include "node.h"

typedef struct StatementUnion StatementUnion;

typedef struct NullStatement NullStatement;
struct NullStatement {
  int dummy;
};

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

typedef struct SwitchStatement SwitchStatement;
struct SwitchStatement {
  Node *condition;
  StatementUnion *statement;
  Vector *labeledStatements; // LabeledStatement vector
};

typedef struct LabeledStatement LabeledStatement;
struct LabeledStatement {
  Node *constantExpression; // caseラベルのときのみ使う
  StatementUnion *statement;
  const String *name; // defaultラベルやcaseラベルでは使わない
};

typedef struct WhileStatement WhileStatement;
struct WhileStatement {
  Node *condition;
  StatementUnion *statement;
};

typedef struct DoWhileStatement DoWhileStatement;
struct DoWhileStatement {
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

typedef struct BlockItem BlockItem;
struct BlockItem {
  StatementUnion *statement;
  Declaration *declaration;
};
typedef struct CompoundStatement CompoundStatement;
struct CompoundStatement {
  Vector *blockItemList; // BlockItem
};

typedef struct BreakStatement BreakStatement;
struct BreakStatement {
  int dummy;
};

typedef struct ContinueStatement ContinueStatement;
struct ContinueStatement {
  int dummy;
};

typedef struct GotoStatement GotoStatement;
struct GotoStatement {
  const String *label;
};

StatementUnion *new_statement_union_null(NullStatement *statement);
StatementUnion *new_statement_union_expression(ExpressionStatement *statement);
StatementUnion *new_statement_union_return(ReturnStatement *statement);
StatementUnion *new_statement_union_if(IfStatement *statement);
StatementUnion *new_statement_union_switch(SwitchStatement *statement);
StatementUnion *new_statement_union_labeled(LabeledStatement *statement);
StatementUnion *new_statement_union_while(WhileStatement *statement);
StatementUnion *new_statement_union_do_while(DoWhileStatement *statement);
StatementUnion *new_statement_union_for(ForStatement *statement);
StatementUnion *new_statement_union_compound(CompoundStatement *statement);
StatementUnion *new_statement_union_break(BreakStatement *statement);
StatementUnion *new_statement_union_continue(ContinueStatement *statement);
StatementUnion *new_statement_union_goto(GotoStatement *statement);

NullStatement *statement_union_take_null(StatementUnion *statementUnion);
ExpressionStatement *
statement_union_take_expression(StatementUnion *statementUnion);
ReturnStatement *statement_union_take_return(StatementUnion *statementUnion);
IfStatement *statement_union_take_if(StatementUnion *statementUnion);
SwitchStatement *statement_union_take_switch(StatementUnion *statementUnion);
LabeledStatement *statement_union_take_labeled(StatementUnion *statementUnion);
WhileStatement *statement_union_take_while(StatementUnion *statementUnion);
DoWhileStatement *statement_union_take_do_while(StatementUnion *statementUnion);
ForStatement *statement_union_take_for(StatementUnion *statementUnion);
CompoundStatement *
statement_union_take_compound(StatementUnion *statementUnion);
BreakStatement *statement_union_take_break(StatementUnion *statementUnion);
ContinueStatement *
statement_union_take_continue(StatementUnion *statementUnion);
GotoStatement *statement_union_take_goto(StatementUnion *statementUnion);

#endif
