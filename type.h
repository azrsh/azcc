#ifndef TYPE_H
#define TYPE_H

#include <stdlib.h>

typedef enum {
  TYPE_INT,
  TYPE_CHAR,
  TYPE_PTR,
  TYPE_ARRAY,
  TYPE_STRUCT,
} TypeKind;

typedef struct Type Type;
struct Type {
  TypeKind kind;
  Type *base;
  size_t length; // TypeKindがARRAYのときのみ使用する
};

Type *new_type(TypeKind kind);
int type_to_size(Type *type);
int type_to_stack_size(Type *type);
char *type_kind_to_char(TypeKind kind);
char *type_to_char(Type *type);

#endif
