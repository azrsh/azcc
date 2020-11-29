int assert(int, int, char *);
int printf();

int main() {
  char buffer[100];
  char *source = "adsvjlwrjprjpwgrrpowejoquiyeofhvdvbkhs";
  int count = 39;

  char *to = buffer;
  char *from = source;

  int n = (count + 7) / 8;
  switch (count % 8) {
  case 0:
    do {
      *to++ = *from++;
    case 7:
      *to++ = *from++;
    case 6:
      *to++ = *from++;
    case 5:
      *to++ = *from++;
    case 4:
      *to++ = *from++;
    case 3:
      *to++ = *from++;
    case 2:
      *to++ = *from++;
    case 1:
      *to++ = *from++;
    } while (--n > 0);
  }

  int res = 1;
  for (int i = 0; i < count; i++) {
    res = res && (buffer[i] == source[i]);
  }
  assert(1, res, "Duff's device");

  printf("OK\n");
  return 0;
}
