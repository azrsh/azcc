#ifndef TYPE_H
#define TYPE_H

#include <stdlib.h>

typedef enum { INT, CHAR, PTR, ARRAY } TypeKind;
typedef struct Type Type;
struct Type {
  TypeKind kind;
  Type *base;
  size_t length; // TypeKindがARRAYのときのみ使用する
};

int type_to_size(Type *type);
int type_to_stack_size(Type *type);
char *type_kind_to_char(TypeKind kind);
char *type_to_char(Type *type);

#endif
