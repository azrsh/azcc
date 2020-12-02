#include "container.h"
#include <stdio.h>
#include <string.h>

void assert(int, int, char *);

int main() {
  int source[10];

  HashTable *hashTable = new_hash_table();
  hash_table_store(hashTable, char_to_string("a"), &source[0]);
  hash_table_store(hashTable, char_to_string("b"), &source[1]);
  hash_table_store(hashTable, char_to_string("c"), &source[2]);
  hash_table_store(hashTable, char_to_string("d"), &source[3]);
  hash_table_store(hashTable, char_to_string("e"), &source[4]);
  hash_table_store(hashTable, char_to_string("f"), &source[5]);
  hash_table_store(hashTable, char_to_string("g"), &source[6]);
  hash_table_store(hashTable, char_to_string("h"), &source[7]);
  hash_table_store(hashTable, char_to_string("i"), &source[8]);
  hash_table_store(hashTable, char_to_string("j"), &source[9]);

  assert(1, &source[0] == hash_table_find(hashTable, char_to_string("a")),
         "&source[0] == hash_table_find(char_to_string(\"a\"))");
  assert(1, &source[1] == hash_table_find(hashTable, char_to_string("b")),
         "&source[1] == hash_table_find(char_to_string(\"b\"))");
  assert(1, &source[2] == hash_table_find(hashTable, char_to_string("c")),
         "&source[2] == hash_table_find(char_to_string(\"c\"))");
  assert(1, &source[3] == hash_table_find(hashTable, char_to_string("d")),
         "&source[3] == hash_table_find(char_to_string(\"d\"))");
  assert(1, &source[4] == hash_table_find(hashTable, char_to_string("e")),
         "&source[4] == hash_table_find(char_to_string(\"e\"))");
  assert(1, &source[5] == hash_table_find(hashTable, char_to_string("f")),
         "&source[5] == hash_table_find(char_to_string(\"f\"))");
  assert(1, &source[6] == hash_table_find(hashTable, char_to_string("g")),
         "&source[6] == hash_table_find(char_to_string(\"g\"))");
  assert(1, &source[7] == hash_table_find(hashTable, char_to_string("h")),
         "&source[7] == hash_table_find(char_to_string(\"h\"))");
  assert(1, &source[8] == hash_table_find(hashTable, char_to_string("i")),
         "&source[8] == hash_table_find(char_to_string(\"i\"))");
  assert(1, &source[9] == hash_table_find(hashTable, char_to_string("j")),
         "&source[9] == hash_table_find(char_to_string(\"j\"))");
  assert(0, hash_table_find(hashTable, char_to_string("k")) != NULL,
         "hash_table_find(char_to_string(\"k\"))");
  assert(0, hash_table_find(hashTable, char_to_string("l")) != NULL,
         "hash_table_find(char_to_string(\"l\"))");
  assert(0, hash_table_find(hashTable, char_to_string("m")) != NULL,
         "hash_table_find(char_to_string(\"m\"))");
  assert(0, hash_table_find(hashTable, char_to_string("n")) != NULL,
         "hash_table_find(char_to_string(\"n\"))");
  assert(0, hash_table_find(hashTable, char_to_string("o")) != NULL,
         "hash_table_find(char_to_string(\"o\"))");

  assert(1, hash_table_contain(hashTable, char_to_string("a")),
         "hash_table_contain(char_to_string(\"a\"))");
  assert(1, hash_table_contain(hashTable, char_to_string("b")),
         "hash_table_contain(char_to_string(\"b\"))");
  assert(1, hash_table_contain(hashTable, char_to_string("c")),
         "hash_table_contain(char_to_string(\"c\"))");
  assert(1, hash_table_contain(hashTable, char_to_string("d")),
         "hash_table_contain(char_to_string(\"d\"))");
  assert(1, hash_table_contain(hashTable, char_to_string("e")),
         "hash_table_contain(char_to_string(\"e\"))");
  assert(1, hash_table_contain(hashTable, char_to_string("f")),
         "hash_table_contain(char_to_string(\"f\"))");
  assert(1, hash_table_contain(hashTable, char_to_string("g")),
         "hash_table_contain(char_to_string(\"g\"))");
  assert(1, hash_table_contain(hashTable, char_to_string("h")),
         "hash_table_contain(char_to_string(\"h\"))");
  assert(1, hash_table_contain(hashTable, char_to_string("i")),
         "hash_table_contain(char_to_string(\"i\"))");
  assert(1, hash_table_contain(hashTable, char_to_string("j")),
         "hash_table_contain(char_to_string(\"j\"))");

  return 0;
}
