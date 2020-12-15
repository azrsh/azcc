#ifndef TAGCONTAINER
#define TAGCONTAINER

#include "container.h"
#include "type.h"

typedef struct Tag Tag;
struct Tag {
  Type *type;
};

typedef struct TagContainer TagContainer;
TagContainer *new_tag_container(ListNode *tableHead);
Tag *tag_container_get(TagContainer *container, const String *name);
bool tag_container_push(TagContainer *container, Tag *tag);
TagContainer *tag_container_push_table(TagContainer *container,
                                       HashTable *table);

#endif
