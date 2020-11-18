#include "type.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>

int type_to_size(Type *type) {
  switch (type->kind) {
  case CHAR:
    return 1;
  case INT:
    return 4;
  case PTR:
    return 8;
  case ARRAY:
    return type_to_size(type->base) * type->length;
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
  case CHAR:
    return "char";
  case INT:
    return "int";
  case PTR:
    return "Pointer";
  case ARRAY:
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
