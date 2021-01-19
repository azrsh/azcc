#ifndef TYPECONTAINER
#define TYPECONTAINER

#include "container.h"
#include "type.h"

typedef struct TypeContainer TypeContainer;
TypeContainer *new_type_container(ListNode *tableHead);
Type *type_container_get(TypeContainer *container, const String *key);
bool type_container_push(TypeContainer *container, const String *key,
                         Type *type);
TypeContainer *type_container_push_table(TypeContainer *container,
                                         HashTable *table);

#endif
