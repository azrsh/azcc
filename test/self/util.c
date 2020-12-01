# 1 "test/self/container.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "test/self/container.c"
# 1 "test/self/container.h" 1



# 1 "test/self/util.h" 1



# 1 "/usr/lib/gcc/x86_64-linux-gnu/9/include/stdbool.h" 1 3 4
# 5 "test/self/util.h" 2


# 6 "test/self/util.h" 3 4
_Bool 
# 6 "test/self/util.h"
    start_with(const char *p, const char *q);




void error();

extern const char *user_input;
extern const char *filename;




void error_at();

const char *read_file(const char *path);
# 5 "test/self/container.h" 2


typedef struct String String;
struct String {
  const char *head;
  int length;
};
String new_string(const char *source, int length);
String char_to_string(const char *source);
char *string_to_char(String source);

# 15 "test/self/container.h" 3 4
_Bool 
# 15 "test/self/container.h"
    string_compare(String string1, String string2);

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
HashTable *new_hash_table();
int hash_table_store(HashTable *table, String key, void *data);

# 38 "test/self/container.h" 3 4
_Bool 
# 38 "test/self/container.h"
    hash_table_contain(HashTable *table, String key);
void *hash_table_find(HashTable *table, String key);
# 2 "test/self/container.c" 2
# 1 "test/self/dummylib/assert.h" 1
# 3 "test/self/container.c" 2

# 1 "test/self/dummylib/stdio.h" 1
# 5 "test/self/container.c" 2
# 1 "test/self/dummylib/stdlib.h" 1



typedef int size_t;
void *calloc(size_t nmemb, size_t size);
# 6 "test/self/container.c" 2
# 1 "test/self/dummylib/string.h" 1







size_t strlen(const char *s);
void *memcpy(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
# 7 "test/self/container.c" 2

String new_string(const char *source, int length) {
  String string;
  string.head = source;
  string.length = length;
  return string;
}

String char_to_string(const char *source) {
  return new_string(source, strlen(source));
}

char *string_to_char(String source) {
  char *p = calloc(source.length + 1, sizeof(char));
  memcpy(p, source.head, source.length);
  p[source.length] = '\0';
  return p;
}


# 26 "test/self/container.c" 3 4
_Bool 
# 26 "test/self/container.c"
    string_compare(String string1, String string2) {
  if (string1.length != string2.length) {
    return 
# 28 "test/self/container.c" 3 4
          0
# 28 "test/self/container.c"
               ;
  }

  return memcmp(string1.head, string2.head, string1.length) == 0;
}




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




struct Vector {
  void **head;
  int length;
  int reserved;
};

Vector *new_vector(int initialSize) {
  Vector *vector = calloc(1, sizeof(Vector));
  vector->head = calloc(initialSize, sizeof(void *));
  vector->reserved = initialSize;
  return vector;
}

void vector_push_back(Vector *vector, void *element) {
  if (vector->length >= vector->reserved) {
    void **old = vector->head;
    void **new = calloc(vector->reserved * 2, sizeof(void *));
    for (int i = 0; i < vector->length; i++) {
      new[i] = old[i];
    }
    vector->head = new;
    vector->reserved *= 2;
  }

  vector->head[vector->length] = element;
  vector->length++;
}

void *vector_get(Vector *vector, int index) {
  if (index < 0 || index >= vector->length) {
    error("配列外参照エラー");
  }

  return vector->head[index];
}
void vector_set(Vector *vector, int index, void *element) {
  if (index < 0 || index >= vector->length) {
    error("配列外参照エラー");
  }

  vector->head[index] = element;
}

int vector_length(Vector *vector) { return vector->length; }




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
  list_push_back(node, pair);



  table->table[keyhash] = head.next;

  return keyhash;
}


# 160 "test/self/container.c" 3 4
_Bool 
# 160 "test/self/container.c"
    hash_table_contain(HashTable *table, String key) {
  return hash_table_find(table, key) != 0;
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
  return 0;
}
