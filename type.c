#include "type.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>

Type *new_type(TypeKind kind) {
  Type *type = calloc(1, sizeof(Type));
  type->kind = kind;
  return type;
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

int type_to_stack_size(Type *type) {
  int size = type_to_size(type);
  size += 8 - size % 8;
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
