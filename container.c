#include "9cc.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

String new_string(char *source, int length) {
  String string;
  string.head = source;
  string.length = length;
  return string;
}

String char_to_string(char *source) {
  return new_string(source, strlen(source));
}

char *string_to_char(String source) {
  char *p = calloc(source.length + 1, sizeof(char));
  memcpy(p, source.head, source.length);
  p[source.length] = '\0';
  return p;
}

bool string_compare(String string1, String string2) {
  if (string1.length != string2.length) {
    return false;
  }

  return memcmp(string1.head, string2.head, string1.length) == 0;
}

ListNode *new_list_node(void *body, ListNode *current) {
  ListNode *node = calloc(1, sizeof(ListNode));
  node->body = body;
  current->next = node;
  return node;
}

//
// HashTable
//
struct HashTable {
  ListNode *table[256];
};

int hash(String source) {
  int sum = 0;
  for (int i = 0; i < source.length; i++) {
    sum += *(source.head + i);
  }
  return sum % 256;
}

typedef struct KeyValuePair KeyValuePair;
struct KeyValuePair {
  String key;
  void *value;
};

KeyValuePair *new_key_value_pair(String key, void *value) {
  KeyValuePair *pair = calloc(1, sizeof(KeyValuePair));
  pair->key = key;
  pair->value = value;
  return pair;
}

HashTable *new_hash_table() { return calloc(1, sizeof(HashTable)); }

int hash_table_store(HashTable *table, String key, void *data) {
  const int keyhash = hash(key);

  ListNode head;
  ListNode *node = &head;
  node->next = table->table[keyhash];
  while (node->next) {
    node = node->next;
  }
  KeyValuePair *pair = new_key_value_pair(key, data);
  new_list_node(pair, node);

  //あるkeyhashに初めて要素が追加されたとき、table[keyhash]は空なので更新する
  //初めてでないときに更新しても問題ないので場合分けはしない
  table->table[keyhash] = head.next;

  return keyhash;
}

bool hash_table_contain(HashTable *table, String key) {
  return hash_table_find(table, key) != NULL;
}

void *hash_table_find(HashTable *table, String key) {
  const int keyhash = hash(key);

  ListNode *node = table->table[keyhash];
  while (node) {
    KeyValuePair *pair = node->body;
    if (string_compare(pair->key, key)) {
      return pair->value;
    }
    node = node->next;
  }
  return NULL;
}
