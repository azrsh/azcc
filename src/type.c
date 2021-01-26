#include "type.h"
#include "container.h"
#include "membercontainer.h"
#include "util.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Type *new_type(TypeKind kind) {
  Type *type = calloc(1, sizeof(Type));
  type->kind = kind;
  return type;
}

bool type_is_primitive(Type *type) {
  switch (type->kind) {
  case TYPE_VOID:
  case TYPE_CHAR:
  case TYPE_INT:
  case TYPE_BOOL:
  case TYPE_ENUM:
    return true;
  case TYPE_PTR:
  case TYPE_ARRAY:
  case TYPE_STRUCT:
  case TYPE_UNION:
  case TYPE_FUNC:
    return false;
  }

  ERROR("予期しない型が指定されました");
  return false;
}

int type_to_size(Type *type) {
  switch (type->kind) {
  case TYPE_CHAR:
  case TYPE_VOID:
  case TYPE_BOOL:
    return 1;
  case TYPE_INT:
  case TYPE_ENUM:
    return 4;
  case TYPE_PTR:
  case TYPE_FUNC: //関数型は常に関数ポインタなので
    return 8;
  case TYPE_ARRAY:
    return type_to_size(type->base) * type->length;
  case TYPE_STRUCT:
  case TYPE_UNION:
    if (!type->isDefined)
      ERROR("未定義の型のサイズは取得できません");
    return member_container_aligned_size(type->members);
  }

  ERROR("予期しない型が指定されました");
  return 0;
}

int type_to_align(Type *type) {
  switch (type->kind) {
  case TYPE_CHAR:
  case TYPE_VOID:
  case TYPE_BOOL:
    return 1;
  case TYPE_INT:
  case TYPE_ENUM:
    return 4;
  case TYPE_PTR:
    return 8;
  case TYPE_ARRAY:
    return type_to_align(type->base);
  case TYPE_STRUCT:
  case TYPE_UNION:
    if (!type->isDefined)
      ERROR("未定義の型のサイズは取得できません");
    return member_container_align(type->members);
  case TYPE_FUNC:
    ERROR("関数型のサイズは取得できません");
    return 0;
  }

  ERROR("予期しない型が指定されました");
  return 0;
}

int type_to_stack_size(Type *type) {
  int size = type_to_size(type);
  size += (8 - size % 8) % 8;
  return size;
}

char *type_kind_to_syntactic_string(TypeKind kind) {
  switch (kind) {
  case TYPE_CHAR:
    return "char";
  case TYPE_VOID:
    return "void";
  case TYPE_INT:
  case TYPE_ENUM:
    return "int";
  case TYPE_BOOL:
    return "_Bool";
  case TYPE_STRUCT:
    return "struct";
  case TYPE_UNION:
    return "union";
  case TYPE_PTR:
    return "Pointer";
  case TYPE_ARRAY:
    return "Array";
  case TYPE_FUNC:
    return "Function";
  }

  ERROR("予期しない型が指定されました");
  return NULL;
}

char *type_to_syntactic_string(Type *type) {
  if (!type)
    return NULL;
  int depth = 1;
  while (type->base) {
    depth++;
    type = type->base;
  }

  char *baseTypeChar = type_kind_to_syntactic_string(type->kind);
  int baseTypeLength = strlen(type_kind_to_syntactic_string(type->kind));
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

char *type_kind_to_semantic_string(TypeKind kind) {
  switch (kind) {
  case TYPE_CHAR:
    return "char";
  case TYPE_VOID:
    return "void";
  case TYPE_INT:
  case TYPE_ENUM:
    return "int";
  case TYPE_BOOL:
    return "_Bool";
  case TYPE_STRUCT:
    return "struct";
  case TYPE_UNION:
    return "union";
  case TYPE_PTR:
    return "Pointer";
  case TYPE_ARRAY:
    return "Array";
  case TYPE_FUNC:
    return "Function";
  }

  ERROR("予期しない型が指定されました");
  return NULL;
}

char *type_to_semantic_string(Type *type) {
  assert(type);

  const int capacity = 128;
  char *buffer = calloc(capacity + 1, sizeof(char));

  char *kind = type_kind_to_semantic_string(type->kind);
  switch (type->kind) {
  case TYPE_CHAR:
  case TYPE_VOID:
  case TYPE_INT:
  case TYPE_BOOL:
    sprintf(buffer, "%s", kind);
    break;
  case TYPE_ENUM:
  case TYPE_STRUCT:
  case TYPE_UNION: {
    const int limit = capacity - strlen(kind) - 1;
    char *tagName = type->name ? string_to_char(type->name) // move from callee
                               : "anonymous";
    sprintf(buffer, "%s %.*s", kind, limit, tagName);
    if (type->name)
      free(tagName); // free own memory
    break;
  }
  case TYPE_PTR: {
    const int limit = capacity - strlen(kind) - 4;
    char *base = type_to_semantic_string(type->base); // move from callee
    sprintf(buffer, "%s to %.*s", kind, limit, base);
    free(base); // free own memory
    break;
  }
  case TYPE_ARRAY: {
    const int limit = capacity - strlen(kind) - 4;
    char *base = type_to_semantic_string(type->base); // move from callee
    sprintf(buffer, "%s of %.*s", kind, limit, base);
    free(base); // free own memory
    break;
  }
  case TYPE_FUNC: {
    const int limit = capacity - strlen(kind) - 11;
    char *returnType =
        type_to_semantic_string(type->returnType); // move from callee
    sprintf(buffer, "%s returning %.*s", kind, limit, returnType);
    free(returnType); // free own memory
    break;
  }
  }

  return buffer; // move to caller
}

bool type_compare_deep(const Type *type1, const Type *type2) {
  if (!type1 && !type2)
    return true;
  if (!type1 || !type2)
    return false;

  //構造体のときは名前で判定する
  if (type1->kind == TYPE_STRUCT && type2->kind == TYPE_STRUCT)
    return string_compare(type1->name, type2->name);

  return type1 && type2 && type1->kind == type2->kind &&
         type1->length == type2->length &&
         type_compare_deep(type1->base, type2->base);
}

bool type_compare_deep_with_implicit_cast(Type *advantage, Type *disadvantage) {
  if (type_compare_deep(advantage, disadvantage))
    return true;
  if (!advantage || !disadvantage)
    return false;

  if (type_is_primitive(advantage) && type_is_primitive(disadvantage))
    return true;

  // voidポインタと他の型のポインタの相互キャストを許可
  if (advantage->kind == TYPE_PTR && disadvantage->kind == TYPE_PTR &&
      (advantage->base->kind == TYPE_VOID ||
       disadvantage->base->kind == TYPE_VOID))
    return true;

  //関数から関数ポインタへの暗黙の変換を許可
  if (advantage->kind == TYPE_PTR && advantage->base->kind == TYPE_FUNC &&
      disadvantage->kind == TYPE_FUNC)
    return true;

  if (type_compare_deep_with_implicit_cast(advantage->base, disadvantage->base))
    return true;

  return false;
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
