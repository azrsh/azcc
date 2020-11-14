#include <stdlib.h>

void alloc4(int **p, int v1, int v2, int v3, int v4) {
  *p = calloc(4, sizeof(int));
  (*p)[0] = v1;
  (*p)[1] = v2;
  (*p)[2] = v3;
  (*p)[3] = v4;
}
