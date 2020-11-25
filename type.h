#ifndef TYPE_H
#define TYPE_H

#include "container.h"
#include <stdlib.h>

typedef struct MemberContainer MemberContainer;

typedef enum {
  TYPE_INT,
  TYPE_CHAR,
  TYPE_PTR,
  TYPE_ARRAY,
  TYPE_STRUCT,
} TypeKind;

typedef struct Type Type;
struct Type {
  String name; //とりあえずはTYPE_STRUCTのときのみ使用
  TypeKind kind;
  Type *base;
  size_t length;            // TypeKindがTYPE_ARRAYのときのみ使用する
  MemberContainer *members; // TypeKindがTYPE_STRUCTのときのみ使用する
};

Type *new_type(TypeKind kind);
int type_to_size(Type *type);
int type_to_align(Type *type);
int type_to_stack_size(Type *type);
char *type_kind_to_char(TypeKind kind);
char *type_to_char(Type *type);

#endif
