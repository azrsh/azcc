#include "testtool.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void memalloc() {
  int *array = calloc(10, sizeof(int));
  for (int i = 0; i < 10; i++) {
    array[i] = i;
    test_assert(i, array[i], "i == array[i]");
  }
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct String String;
struct String {
  const char *head;
  int length;
};

typedef struct ListNode ListNode;
struct ListNode {
  void *body;
  ListNode *next;
};

typedef struct HashTable HashTable;

const String *new_string(const char *source, int length) {
  String *string = calloc(1, sizeof(String));
  string->head = source;
  string->length = length;
  return string;
}

const String *char_to_string(const char *source) {
  return new_string(source, strlen(source));
}

char *string_to_char(const String *source) {
  char *p = calloc(source->length + 1, sizeof(char));
  memcpy(p, source->head, source->length);
  p[source->length] = '\0';
  return p;
}

bool string_compare(const String *string1, const String *string2) {
  if (string1->length != string2->length) {
    return false;
  }

  return memcmp(string1->head, string2->head, string1->length) == 0;
}

//
// List
//
ListNode *new_list_node(void *body) {
  ListNode *node = calloc(1, sizeof(ListNode));
  node->body = body;
  return node;
}

ListNode *list_head_to_tail(ListNode *head) {
  while (head->next)
    head = head->next;
  return head;
}

void list_merge(ListNode *list1, ListNode *list2) { list1->next = list2; }

ListNode *list_push_back(ListNode *tail, void *element) {
  ListNode *node = new_list_node(element);
  tail->next = node;
  return node;
}

ListNode *list_push_front(ListNode *head, void *element) {
  ListNode *node = new_list_node(element);
  node->next = head;
  return node;
}

//
// HashTable
//
struct HashTable {
  ListNode *table[256];
};

int hash(const String *source) {
  int sum = 0;
  for (int i = 0; i < source->length; i++) {
    sum += *(source->head + i);
  }
  return sum % 256;
}

typedef struct KeyValuePair KeyValuePair;
struct KeyValuePair {
  const String *key;
  void *value;
};

KeyValuePair *new_key_value_pair(const String *key, void *value) {
  KeyValuePair *pair = calloc(1, sizeof(KeyValuePair));
  pair->key = key;
  pair->value = value;
  return pair;
}

HashTable *new_hash_table() { return calloc(1, sizeof(HashTable)); }

void hash_table_store(HashTable *table, const String *key, void *data) {
  const int keyhash = hash(key);

  ListNode head;
  ListNode *node = &head;
  node->next = table->table[keyhash];
  while (node->next) {
    node = node->next;
  }
  KeyValuePair *pair = new_key_value_pair(key, data);
  list_push_back(node, pair);

  //あるkeyhashに初めて要素が追加されたとき、table[keyhash]は空なので更新する
  //初めてでないときに更新しても問題ないので場合分けはしない
  table->table[keyhash] = head.next;
}

void *hash_table_find(HashTable *table, const String *key) {
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

bool hash_table_contain(HashTable *table, const String *key) {
  return hash_table_find(table, key) != NULL;
}

void test_malloc_source() {
  printf("malloc source test\n");
  int *source[10];
  for (int i = 0; i < 10; i++) {
    source[i] = calloc(1, sizeof(int));
  }

  HashTable *hashTable = new_hash_table();
  hash_table_store(hashTable, char_to_string("a"), source[0]);
  hash_table_store(hashTable, char_to_string("b"), source[1]);
  hash_table_store(hashTable, char_to_string("c"), source[2]);
  hash_table_store(hashTable, char_to_string("d"), source[3]);
  hash_table_store(hashTable, char_to_string("e"), source[4]);
  hash_table_store(hashTable, char_to_string("f"), source[5]);
  hash_table_store(hashTable, char_to_string("g"), source[6]);
  hash_table_store(hashTable, char_to_string("h"), source[7]);
  hash_table_store(hashTable, char_to_string("i"), source[8]);
  hash_table_store(hashTable, char_to_string("j"), source[9]);

  test_assert(1, source[0] == hash_table_find(hashTable, char_to_string("a")),
         "source[0] == hash_table_find(char_to_string(\"a\"))");
  test_assert(1, source[1] == hash_table_find(hashTable, char_to_string("b")),
         "source[1] == hash_table_find(char_to_string(\"b\"))");
  test_assert(1, source[2] == hash_table_find(hashTable, char_to_string("c")),
         "source[2] == hash_table_find(char_to_string(\"c\"))");
  test_assert(1, source[3] == hash_table_find(hashTable, char_to_string("d")),
         "source[3] == hash_table_find(char_to_string(\"d\"))");
  test_assert(1, source[4] == hash_table_find(hashTable, char_to_string("e")),
         "source[4] == hash_table_find(char_to_string(\"e\"))");
  test_assert(1, source[5] == hash_table_find(hashTable, char_to_string("f")),
         "source[5] == hash_table_find(char_to_string(\"f\"))");
  test_assert(1, source[6] == hash_table_find(hashTable, char_to_string("g")),
         "source[6] == hash_table_find(char_to_string(\"g\"))");
  test_assert(1, source[7] == hash_table_find(hashTable, char_to_string("h")),
         "source[7] == hash_table_find(char_to_string(\"h\"))");
  test_assert(1, source[8] == hash_table_find(hashTable, char_to_string("i")),
         "source[8] == hash_table_find(char_to_string(\"i\"))");
  test_assert(1, source[9] == hash_table_find(hashTable, char_to_string("j")),
         "source[9] == hash_table_find(char_to_string(\"j\"))");
  test_assert(0, hash_table_find(hashTable, char_to_string("k")) != NULL,
         "hash_table_find(char_to_string(\"k\"))");
  test_assert(0, hash_table_find(hashTable, char_to_string("l")) != NULL,
         "hash_table_find(char_to_string(\"l\"))");
  test_assert(0, hash_table_find(hashTable, char_to_string("m")) != NULL,
         "hash_table_find(char_to_string(\"m\"))");
  test_assert(0, hash_table_find(hashTable, char_to_string("n")) != NULL,
         "hash_table_find(char_to_string(\"n\"))");
  test_assert(0, hash_table_find(hashTable, char_to_string("o")) != NULL,
         "hash_table_find(char_to_string(\"o\"))");

  test_assert(1, hash_table_contain(hashTable, char_to_string("a")),
         "hash_table_contain(char_to_string(\"a\"))");
  test_assert(1, hash_table_contain(hashTable, char_to_string("b")),
         "hash_table_contain(char_to_string(\"b\"))");
  test_assert(1, hash_table_contain(hashTable, char_to_string("c")),
         "hash_table_contain(char_to_string(\"c\"))");
  test_assert(1, hash_table_contain(hashTable, char_to_string("d")),
         "hash_table_contain(char_to_string(\"d\"))");
  test_assert(1, hash_table_contain(hashTable, char_to_string("e")),
         "hash_table_contain(char_to_string(\"e\"))");
  test_assert(1, hash_table_contain(hashTable, char_to_string("f")),
         "hash_table_contain(char_to_string(\"f\"))");
  test_assert(1, hash_table_contain(hashTable, char_to_string("g")),
         "hash_table_contain(char_to_string(\"g\"))");
  test_assert(1, hash_table_contain(hashTable, char_to_string("h")),
         "hash_table_contain(char_to_string(\"h\"))");
  test_assert(1, hash_table_contain(hashTable, char_to_string("i")),
         "hash_table_contain(char_to_string(\"i\"))");
  test_assert(1, hash_table_contain(hashTable, char_to_string("j")),
         "hash_table_contain(char_to_string(\"j\"))");
  test_assert(0, hash_table_contain(hashTable, char_to_string("k")),
         "hash_table_contain(char_to_string(\"k\"))");
  test_assert(0, hash_table_contain(hashTable, char_to_string("l")),
         "hash_table_contain(char_to_string(\"l\"))");
  test_assert(0, hash_table_contain(hashTable, char_to_string("m")),
         "hash_table_contain(char_to_string(\"m\"))");
  test_assert(0, hash_table_contain(hashTable, char_to_string("n")),
         "hash_table_contain(char_to_string(\"n\"))");
  test_assert(0, hash_table_contain(hashTable, char_to_string("o")),
         "hash_table_contain(char_to_string(\"o\"))");
  printf("OK\n");
}

int main() {
  memalloc();
  test_malloc_source();
  printf("OK\n");
  return 0;
}
