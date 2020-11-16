#ifndef VARIABLECONTAINER
#define VARIABLECONTAINER

#include "container.h"
#include "localvariable.h"

typedef struct VariableContainer VariableContainer;
VariableContainer *new_variable_container(ListNode *tableHead);
LocalVariable *variable_container_get(VariableContainer *container,
                                      String name);
bool variable_container_push(VariableContainer *container,
                             LocalVariable *varibale);
bool variable_container_update(VariableContainer *container,
                               LocalVariable *variable);
VariableContainer *variable_container_push_table(VariableContainer *container,
                                                 HashTable *table);

#endif
