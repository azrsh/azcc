#include "membercontainer.h"
#include "container.h"
#include "type.h"
#include "variable.h"

struct MemberContainer {
  HashTable *table;
  Vector *vector;
  size_t maxSize;
};

MemberContainer *new_member_container() {
  MemberContainer *container = calloc(1, sizeof(MemberContainer));
  container->table = new_hash_table();
  container->vector = new_vector(16);
  return container;
}

Variable *member_container_get(MemberContainer *container, String name) {
  return hash_table_find(container->table, name);
}

bool member_container_push(MemberContainer *container, Variable *variable) {
  bool isExist = hash_table_contain(container->table, variable->name);
  if (isExist)
    return false;

  hash_table_store(container->table, variable->name, variable);
  vector_push_back(container->vector, variable);
  return true;
}

size_t member_container_align(MemberContainer *container) {
  size_t alignMax = 0;
  int numberOfMembers = vector_length(container->vector);
  for (int i = 0; i < numberOfMembers; i++) {
    Variable *variable = vector_get(container->vector, i);
    size_t align = type_to_align(variable->type);
    if (alignMax < align)
      alignMax = align;
  }
  return alignMax;
}

size_t member_container_aligned_size(MemberContainer *container) {
  int numberOfMembers = vector_length(container->vector);
  size_t alignment = member_container_align(container);
  Variable *endVariable = vector_get(container->vector, numberOfMembers - 1);
  int addressEnd = endVariable->offset + type_to_size(endVariable->type);
  return addressEnd + (alignment - addressEnd % alignment) % alignment;
}
