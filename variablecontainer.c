#include "variablecontainer.h"
#include "variable.h"

struct VariableContainer {
  ListNode *tableHead;
};

VariableContainer *new_variable_container(ListNode *tableHead) {
  VariableContainer *container = calloc(1, sizeof(VariableContainer));
  container->tableHead = tableHead;
  return container;
}

Variable *variable_container_get(VariableContainer *container, String name) {
  ListNode *list = container->tableHead;
  while (list) {
    Variable *variable = hash_table_find(list->body, name);
    if (variable)
      return variable;

    list = list->next;
  }

  return NULL;
}

bool variable_container_push(VariableContainer *container, Variable *variable) {
  HashTable *localTable = container->tableHead->body;
  bool isExist = hash_table_contain(localTable, variable->name);
  if (isExist)
    return false;

  hash_table_store(localTable, variable->name, variable);
  return true;
}

bool variable_container_update(VariableContainer *container,
                               Variable *variable) {
  if (variable_container_get(container, variable->name)) {
    HashTable *localTable = container->tableHead->body;
    hash_table_store(localTable, variable->name, variable);
    return true;
  }

  return false;
}

VariableContainer *variable_container_push_table(VariableContainer *container,
                                                 HashTable *table) {
  ListNode *newHead = list_push_front(container->tableHead, table);
  return new_variable_container(newHead);
}
