#include "type.h"
#include "membercontainer.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>

Type *new_type(TypeKind kind) {
  Type *type = calloc(1, sizeof(Type));
  type->kind = kind;
  return type;
}

bool type_is_primitive(Type *type) {
  switch (type->kind) {
  case TYPE_CHAR:
  case TYPE_INT:
    return true;
  }

  return false;
}

int type_to_size(Type *type) {
  switch (type->kind) {
  case TYPE_CHAR:
    return 1;
  case TYPE_INT:
    return 4;
  case TYPE_PTR:
    return 8;
  case TYPE_ARRAY:
    return type_to_size(type->base) * type->length;
  case TYPE_STRUCT:
    return member_container_aligned_size(type->members);
  }

  error("予期しない型が指定されました");
  return 0;
}

int type_to_align(Type *type) {
  switch (type->kind) {
  case TYPE_CHAR:
    return 1;
  case TYPE_INT:
    return 4;
  case TYPE_PTR:
    return 8;
  case TYPE_ARRAY:
    return type_to_size(type->base);
  case TYPE_STRUCT:
    return member_container_align(type->members);
  }

  error("予期しない型が指定されました");
  return 0;
}

int type_to_stack_size(Type *type) {
  int size = type_to_size(type);
  size += (8 - size % 8) % 8;
  return size;
}

char *type_kind_to_char(TypeKind kind) {
  switch (kind) {
  case TYPE_CHAR:
    return "char";
  case TYPE_INT:
    return "int";
  case TYPE_PTR:
    return "Pointer";
  case TYPE_ARRAY:
    return "Array";
  }
}

char *type_to_char(Type *type) {
  if (!type)
    return NULL;
  int depth = 1;
  while (type->base) {
    depth++;
    type = type->base;
  }

  char *baseTypeChar = type_kind_to_char(type->kind);
  int baseTypeLength = strlen(type_kind_to_char(type->kind));
  int length = depth + baseTypeLength - 1 + 1;
  char *result = calloc(length, sizeof(char));
  for (int i = 0; i < length - 1; i++) {
    if (i < baseTypeLength)
      result[i] = baseTypeChar[i];
    else
      result[i] = '*';
  }
  result[length - 1] = '\0';
  return result;
}

bool type_compare_deep(const Type *type1, const Type *type2) {
  return (!type1 && !type2) || (type1 && type2 && type1->kind == type2->kind &&
                                type1->length == type2->length &&
                                type_compare_deep(type1->base, type2->base));
}

bool type_vector_compare(Vector *typeVector1, Vector *typeVector2) {
  if (vector_length(typeVector1) != vector_length(typeVector2))
    return false;

  for (int i = 0; i < vector_length(typeVector1); i++) {
    Type *type1 = vector_get(typeVector1, i);
    Type *type2 = vector_get(typeVector2, i);
    if (!type_compare_deep(type1, type2))
      return false;
  }

  return true;
}
