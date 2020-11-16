#include "type.h"
#include "util.h"

int type_to_size(Type *type) {
  switch (type->kind) {
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
  size += size % 8;
  return size;
}
