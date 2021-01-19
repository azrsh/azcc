#include "typecontainer.h"
#include "container.h"

struct TypeContainer {
  ListNode *tableHead;
};

TypeContainer *new_type_container(ListNode *tableHead) {
  TypeContainer *container = calloc(1, sizeof(TypeContainer));
  container->tableHead = tableHead;
  return container;
}

Type *type_container_get(TypeContainer *container, const String *key) {
  ListNode *list = container->tableHead;
  while (list) {
    Type *tag = hash_table_find(list->body, key);
    if (tag)
      return tag;

    list = list->next;
  }

  return NULL;
}

bool type_container_push(TypeContainer *container, const String *key,
                         Type *type) {
  HashTable *localTable = container->tableHead->body;
  bool isExist = hash_table_contain(localTable, key);
  if (isExist)
    return false;

  hash_table_store(localTable, key, type);
  return true;
}

TypeContainer *type_container_push_table(TypeContainer *container,
                                         HashTable *table) {
  ListNode *newHead = list_push_front(container->tableHead, table);
  return new_type_container(newHead);
}
