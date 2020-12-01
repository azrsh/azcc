#include "container.h"

void assert();

int main() {
  int source[10];
  /*source[0] = 183;
  source[1] = 132;
  source[2] = 18;
  source[3] = 35;
  source[4] = 2456;
  source[5] = 1;
  source[6] = 3213;
  source[7] = 18;
  source[8] = 1823;
  source[9] = 8;*/

  Vector *vector = new_vector(16);
  for (int i = 0; i < sizeof(source) / sizeof(int); i++)
    vector_push_back(vector, &source[i]);

  assert(1, sizeof(source) / sizeof(int) == vector_length(vector),
         "sizeof(source)/sizeof(int) == vector_length(vector)");

  for (int i = 0; i < vector_length(vector); i++) {
    assert(1, source + i == vector_get(vector, i),
           "source + i == vector_get(i)");
  }

  return 0;
}
