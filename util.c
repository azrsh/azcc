#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

bool start_with(char *p, char *q) { return memcmp(p, q, strlen(q)) == 0; }
