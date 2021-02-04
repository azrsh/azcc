extern int putchar(int c);
char buff[10];

void itoa_r(int i, char *s) {
  do {
    *(s++) = ('0' + (i % 10));
  } while ((i = i / 10) != 0);
  *s = 0;
}

void putstr_r(char *s) {
  if (!*s)
    return;
  putstr_r(s + 1);
  putchar(*s);
}

void fizzbuzz(int i) {
  int printed = 0;

  if (i)
    fizzbuzz(i - 1);

  if (!(i % 3) && ++printed)
    putstr_r("zziF");
  if (!(i % 5) && ++printed)
    putstr_r("zzuB");

  if (!printed) {
    itoa_r(i, buff);
    putstr_r(buff);
  }

  putchar('\n');
}

int main() {
  fizzbuzz(20);
  return 0;
}
