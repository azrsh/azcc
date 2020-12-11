#include "testtool.h"
#include <stdbool.h>
#include <stdio.h>

char *string = "test";
int array_int[4] = {1, 2, 3, 4};
char array_char1[4] = {1, 2, 3, 4};
char array_char2[4] = {'t', 'e', 's', 't'};
char *array_string[4] = {"test1", "test2", "test3", "test4"};

void check_string() {
  assert(1, string[0] == 't', "string[0] == \'t\'");
  assert(1, string[1] == 'e', "string[1] == \'e\'");
  assert(1, string[2] == 's', "string[2] == \'s\'");
  assert(1, string[3] == 't', "string[3] == \'t\'");
  assert(1, string[4] == '\0', "string[4] == \'\\0\'");
}

void check_array_int() {
  assert(1, array_int[0], "array_int[0]");
  assert(2, array_int[1], "array_int[1]");
  assert(3, array_int[2], "array_int[2]");
  assert(4, array_int[3], "array_int[3]");
}

void check_array_char1() {
  assert(1, array_char1[0], "array_char1[0]");
  assert(2, array_char1[1], "array_char1[1]");
  assert(3, array_char1[2], "array_char1[2]");
  assert(4, array_char1[3], "array_char1[3]");
}

void check_array_char2() {
  assert('t', array_char2[0], "array_char2[0]");
  assert('e', array_char2[1], "array_char2[1]");
  assert('s', array_char2[2], "array_char2[2]");
  assert('t', array_char2[3], "array_char2[3]");
}
void check_array_string() {
  for (int i = 0; i < 4; i++) {
    assert(1, array_string[i][0] == 't', "string[0] == \'t\'");
    assert(1, array_string[i][1] == 'e', "string[1] == \'e\'");
    assert(1, array_string[i][2] == 's', "string[2] == \'s\'");
    assert(1, array_string[i][3] == 't', "string[3] == \'t\'");
    assert(1, array_string[i][4] == '1' + i, "string[4] == \'i + 1\'");
    assert(1, array_string[i][5] == '\0', "string[4] == \'\\0\'");
  }
}

void test_global() {
  check_string();
  check_array_int();
  check_array_char1();
  check_array_char2();
  check_array_string();

  string[0] = 116;
  string[1] = 101;
  string[2] = 115;
  string[3] = 116;
  string[4] = 0;

  array_int[0] = 1;
  array_int[1] = 2;
  array_int[2] = 3;
  array_int[3] = 4;

  array_char1[0] = 1;
  array_char1[1] = 2;
  array_char1[2] = 3;
  array_char1[3] = 4;

  array_char2[0] = 116;
  array_char2[1] = 101;
  array_char2[2] = 115;
  array_char2[3] = 116;

  array_string[0] = "test1";
  array_string[1] = "test2";
  array_string[2] = "test3";
  array_string[3] = "test4";

  check_string();
  check_array_int();
  check_array_char1();
  check_array_char2();
  check_array_string();
}

void test_local() {
  /*
  // initialize
  char *string = "test";
  int array_int[4] = {1, 2, 3, 4};
  char array_char1[4] = {1, 2, 3, 4};
  char array_char2[4] = {'t', 'e', 's', 't'};
  char *array_string[4] = {"test1", "test2", "test3", "test4"};

  // check
  assert(1, string[0] == 't', "string[0] == \'t\'");
  assert(1, string[1] == 'e', "string[1] == \'e\'");
  assert(1, string[2] == 's', "string[2] == \'s\'");
  assert(1, string[3] == 't', "string[3] == \'t\'");
  assert(1, string[4] == '\0', "string[4] == \'\\0\'");

  assert(1, array_int[0], "array_int[0]");
  assert(2, array_int[1], "array_int[1]");
  assert(3, array_int[2], "array_int[2]");
  assert(4, array_int[3], "array_int[3]");

  assert(1, array_char1[0], "array_char1[0]");
  assert(2, array_char1[1], "array_char1[1]");
  assert(3, array_char1[2], "array_char1[2]");
  assert(4, array_char1[3], "array_char1[3]");

  assert('t', array_char2[0], "array_char2[0]");
  assert('e', array_char2[1], "array_char2[1]");
  assert('s', array_char2[2], "array_char2[2]");
  assert('t', array_char2[3], "array_char2[3]");

  for (int i = 0; i < 4; i++) {
    assert(1, array_string[i][0] == 't', "string[0] == \'t\'");
    assert(1, array_string[i][1] == 'e', "string[1] == \'e\'");
    assert(1, array_string[i][2] == 's', "string[2] == \'s\'");
    assert(1, array_string[i][3] == 't', "string[3] == \'t\'");
    assert(1, array_string[i][4] == '1' + i, "string[4] == \'i + 1\'");
    assert(1, array_string[i][5] == '\0', "string[4] == \'\\0\'");
  }

  // assign
  string[0] = 116;
  string[1] = 101;
  string[2] = 115;
  string[3] = 116;
  string[4] = 0;

  array_int[0] = 1;
  array_int[1] = 2;
  array_int[2] = 3;
  array_int[3] = 4;

  array_char1[0] = 1;
  array_char1[1] = 2;
  array_char1[2] = 3;
  array_char1[3] = 4;

  array_char2[0] = 116;
  array_char2[1] = 101;
  array_char2[2] = 115;
  array_char2[3] = 116;

  array_string[0] = "test1";
  array_string[1] = "test2";
  array_string[2] = "test3";
  array_string[3] = "test4";

  // check
  assert(1, string[0] == 't', "string[0] == \'t\'");
  assert(1, string[1] == 'e', "string[1] == \'e\'");
  assert(1, string[2] == 's', "string[2] == \'s\'");
  assert(1, string[3] == 't', "string[3] == \'t\'");
  assert(1, string[4] == '\0', "string[4] == \'\\0\'");

  assert(1, array_int[0], "array_int[0]");
  assert(2, array_int[1], "array_int[1]");
  assert(3, array_int[2], "array_int[2]");
  assert(4, array_int[3], "array_int[3]");

  assert(1, array_char1[0], "array_char1[0]");
  assert(2, array_char1[1], "array_char1[1]");
  assert(3, array_char1[2], "array_char1[2]");
  assert(4, array_char1[3], "array_char1[3]");

  assert('t', array_char2[0], "array_char2[0]");
  assert('e', array_char2[1], "array_char2[1]");
  assert('s', array_char2[2], "array_char2[2]");
  assert('t', array_char2[3], "array_char2[3]");

  for (int i = 0; i < 4; i++) {
    assert(1, array_string[i][0] == 't', "string[0] == \'t\'");
    assert(1, array_string[i][1] == 'e', "string[1] == \'e\'");
    assert(1, array_string[i][2] == 's', "string[2] == \'s\'");
    assert(1, array_string[i][3] == 't', "string[3] == \'t\'");
    assert(1, array_string[i][4] == '1' + i, "string[4] == \'i + 1\'");
    assert(1, array_string[i][5] == '\0', "string[4] == \'\\0\'");
  }
  */
}

int main() {
  test_global();
  test_local();
  printf("OK\n");
  return 0;
}
