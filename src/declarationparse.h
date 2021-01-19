#ifndef DECLARATIONPARSE_H
#define DECLARATIONPARSE_H

#include "declaration.h"
#include "parseutil.h"

Declaration *declaration(Declaration *base, ParseContext *context);
Declaration *declaration_specifier(ParseContext *context);
Variable *declarator(Type *base, ParseContext *context);
Type *type_name(ParseContext *context);

#endif
