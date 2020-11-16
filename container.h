#ifndef CONTAINER_H
#define CONTAINER_H

#include "util.h"
#include <stdbool.h>

typedef struct String String;
struct String {
  char *head;
  int length;
};
String new_string(char *source, int length);
String char_to_string(char *source);
char *string_to_char(String source);
bool string_compare(String string1, String string2);

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
int vector_length(Vector *vector);

typedef struct HashTable HashTable;
HashTable *new_hash_table();
int hash_table_store(HashTable *table, String key, void *data);
bool hash_table_contain(HashTable *table, String key);
void *hash_table_find(HashTable *table, String key);

#endif
