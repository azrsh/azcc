#include "tagcontainer.h"

struct TagContainer {
  ListNode *tableHead;
};

TagContainer *new_tag_container(ListNode *tableHead) {
  TagContainer *container = calloc(1, sizeof(TagContainer));
  container->tableHead = tableHead;
  return container;
}

Tag *tag_container_get(TagContainer *container, const String *name) {
  ListNode *list = container->tableHead;
  while (list) {
    Tag *tag = hash_table_find(list->body, name);
    if (tag)
      return tag;

    list = list->next;
  }

  return NULL;
}

bool tag_container_push(TagContainer *container, Tag *tag) {
  HashTable *localTable = container->tableHead->body;
  bool isExist = hash_table_contain(localTable, tag->type->name);
  if (isExist)
    return false;

  hash_table_store(localTable, tag->type->name, tag);
  return true;
}

TagContainer *tag_container_push_table(TagContainer *container,
                                       HashTable *table) {
  ListNode *newHead = list_push_front(container->tableHead, table);
  return new_tag_container(newHead);
}
