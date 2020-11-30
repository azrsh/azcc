#ifndef TYPE_H
#define TYPE_H

#include "container.h"
#include <stdlib.h>

typedef struct MemberContainer MemberContainer;

typedef enum {
  TYPE_INT,
  TYPE_CHAR,
  TYPE_VOID,
  TYPE_BOOL,
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
bool type_is_primitive(Type *type);
int type_to_size(Type *type);
int type_to_align(Type *type);
int type_to_stack_size(Type *type);
char *type_kind_to_char(TypeKind kind);
char *type_to_char(Type *type);
bool type_compare_deep(const Type *type1, const Type *type2);
bool type_vector_compare(Vector *typeVector1, Vector *typeVector2);

#endif
