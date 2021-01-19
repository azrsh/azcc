#ifndef EXPRESSIONPARSE_H
#define EXPRESSIONPARSE_H

#include "node.h"
#include "parseutil.h"

Node *constant_expression(ParseContext *context);
Node *assign(ParseContext *context);

#endif
