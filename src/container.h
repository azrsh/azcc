#ifndef CONTAINER_H
#define CONTAINER_H

#include "util.h"
#include <stdbool.h>

typedef struct String String;
struct String {
  const char *head;
  int length;
};
const String *new_string(const char *source, int length);
const String *char_to_string(const char *source);
char *string_to_char(const String *source);
const String *string_concat(const String *source1, const String *source2);
bool string_compare(const String *string1, const String *string2);

typedef struct ListNode ListNode;
struct ListNode {
  void *body;
  ListNode *next;
};
ListNode *new_list_node(void *body);
ListNode *list_head_to_tail(ListNode *head);
void list_merge(ListNode *list1, ListNode *list2);
ListNode *list_push_back(ListNode *tail, void *element);
ListNode *list_push_front(ListNode *head, void *element);

typedef struct Vector Vector;
Vector *new_vector(int initialSize);
void vector_push_back(Vector *vector, void *element);
void *vector_get(Vector *vector, int index);
void vector_set(Vector *vector, int index, void *element);
int vector_length(Vector *vector);

typedef struct HashTable HashTable;
HashTable *new_hash_table(void);
void hash_table_store(HashTable *table, const String *key, void *data);
bool hash_table_contain(HashTable *table, const String *key);
void *hash_table_find(HashTable *table, const String *key);

#endif
