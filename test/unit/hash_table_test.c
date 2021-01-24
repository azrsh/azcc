#include "container.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_assert(int, int, char *);

void test_array_source() {
  printf("array source test\n");

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

  test_assert(1, &source[0] == hash_table_find(hashTable, char_to_string("a")),
         "&source[0] == hash_table_find(char_to_string(\"a\"))");
  test_assert(1, &source[1] == hash_table_find(hashTable, char_to_string("b")),
         "&source[1] == hash_table_find(char_to_string(\"b\"))");
  test_assert(1, &source[2] == hash_table_find(hashTable, char_to_string("c")),
         "&source[2] == hash_table_find(char_to_string(\"c\"))");
  test_assert(1, &source[3] == hash_table_find(hashTable, char_to_string("d")),
         "&source[3] == hash_table_find(char_to_string(\"d\"))");
  test_assert(1, &source[4] == hash_table_find(hashTable, char_to_string("e")),
         "&source[4] == hash_table_find(char_to_string(\"e\"))");
  test_assert(1, &source[5] == hash_table_find(hashTable, char_to_string("f")),
         "&source[5] == hash_table_find(char_to_string(\"f\"))");
  test_assert(1, &source[6] == hash_table_find(hashTable, char_to_string("g")),
         "&source[6] == hash_table_find(char_to_string(\"g\"))");
  test_assert(1, &source[7] == hash_table_find(hashTable, char_to_string("h")),
         "&source[7] == hash_table_find(char_to_string(\"h\"))");
  test_assert(1, &source[8] == hash_table_find(hashTable, char_to_string("i")),
         "&source[8] == hash_table_find(char_to_string(\"i\"))");
  test_assert(1, &source[9] == hash_table_find(hashTable, char_to_string("j")),
         "&source[9] == hash_table_find(char_to_string(\"j\"))");
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

void test_malloc_source() {
  printf("malloc source test\n");
  int *source[10];
  int sourceVal[10];
  for (int i = 0; i < 10; i++) {
    source[i] = &sourceVal[i]; // calloc(1, sizeof(int));
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
  test_array_source();
  test_malloc_source();
  return 0;
}
