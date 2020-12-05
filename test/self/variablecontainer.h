#ifndef VARIABLECONTAINER
#define VARIABLECONTAINER

#include "container.h"
#include "variable.h"

typedef struct VariableContainer VariableContainer;
VariableContainer *new_variable_container(ListNode *tableHead);
Variable *variable_container_get(VariableContainer *container, String name);
bool variable_container_push(VariableContainer *container, Variable *varibale);
VariableContainer *variable_container_push_table(VariableContainer *container,
                                                 HashTable *table);
size_t variable_container_stack_size(VariableContainer *variableContainer);

#endif
