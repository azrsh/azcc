#ifndef TYPE_H
#define TYPE_H

#include "container.h"
#include <stdlib.h>

typedef struct MemberContainer MemberContainer;

typedef enum { SIGN_NONE, SIGN_SIGNED, SIGN_UNSIGNED } SignKind;

typedef enum {
  TYPE_NONE,
  TYPE_INT,
  TYPE_CHAR,
  TYPE_VOID,
  TYPE_BOOL,
  TYPE_PTR,
  TYPE_ARRAY,
  TYPE_STRUCT,
  TYPE_UNION,
  TYPE_FUNC,
  TYPE_ENUM,
} TypeKind;

typedef struct Type Type;
struct Type {
  SignKind signKind;
  TypeKind kind;
  Type *base;
  size_t length; // TypeKindがTYPE_ARRAYのときのみ使用する

  // strucrt、enum、union
  const String *name; //とりあえずはTYPE_STRUCT、TYPE_ENUMlのときのみ使用
  MemberContainer *members; // TypeKindがTYPE_STRUCTのときのみ使用する
  bool isDefined;           // TypeKindがTYPE_STRUCTのときのみ使用する

  // function
  Type *returnType;
  Vector *arguments; // Declaration Vector
};

Type *new_type(TypeKind kind);
Type *new_type_with_sign(TypeKind typeKind, SignKind signKind);
bool type_is_primitive(Type *type);
int type_to_size(Type *type);
int type_to_align(Type *type);
int type_to_stack_size(Type *type);
char *type_to_syntactic_string(Type *type);
char *type_to_semantic_string(Type *type);
bool type_compare_deep(const Type *type1, const Type *type2);
bool type_compare_deep_with_implicit_cast(Type *advantage, Type *disadvantage);
bool type_vector_compare(Vector *typeVector1, Vector *typeVector2);
bool type_vector_compare_with_implicit_cast(Vector *typeVectorAdvantage,
                                            Vector *typeVectorDisadvantage);

#endif
