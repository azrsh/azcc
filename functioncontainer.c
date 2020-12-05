#include "functioncontainer.h"
#include "container.h"
#include "type.h"

struct FunctionContainer {
  HashTable *table;
};

FunctionContainer *new_function_container() {
  FunctionContainer *container = calloc(1, sizeof(FunctionContainer));
  container->table = new_hash_table();
  return container;
}

FunctionDeclaration *function_container_get(FunctionContainer *container,
                                            const String *name) {
  return hash_table_find(container->table, name);
}

bool function_container_push(FunctionContainer *container,
                             FunctionDeclaration *declaration) {
  bool isExist = hash_table_contain(container->table, declaration->name);
  if (isExist)
    return false;

  hash_table_store(container->table, declaration->name, declaration);
  return true;
}
