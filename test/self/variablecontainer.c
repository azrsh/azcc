#include "variablecontainer.h"
#include "variable.h"

struct VariableContainer {
  ListNode *tableHead;
  size_t stackSize;
};

VariableContainer *new_variable_container(ListNode *tableHead) {
  VariableContainer *container = calloc(1, sizeof(VariableContainer));
  container->tableHead = tableHead;
  return container;
}

Variable *variable_container_get(VariableContainer *container, String name) {
  ListNode *list = container->tableHead;
  while (list) {
    Variable *variable = hash_table_find(list->body, &name);
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
  container->stackSize += type_to_stack_size(variable->type);
  return true;
}

VariableContainer *variable_container_push_table(VariableContainer *container,
                                                 HashTable *table) {
  ListNode *newHead = list_push_front(container->tableHead, table);
  return new_variable_container(newHead);
}

size_t variable_container_stack_size(VariableContainer *variableContainer) {
  return variableContainer->stackSize;
}
