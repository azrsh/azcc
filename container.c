#include "9cc.h"
#include <stdlib.h>

ListNode *new_list_node(void *body, ListNode *current){
  ListNode *node = calloc(1, sizeof(ListNode));
  node->body = body;
  current->next = node;
  return node;
}
