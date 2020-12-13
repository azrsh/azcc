#!/bin/sh

TEMP=$(mktemp)
AZCC="$(cd $(dirname $0) && pwd)/../../bin/gen1/azcc"

set -e
echo "
extern int putchar(int c);
char buff[10];

void itoa_r(int i, char *s) {
  do {
    *(s++) = ('0' + (i % 10));
  } while ((i = i / 10) != 0);
  *s = 0;
}

void putstr_r(char *s) {
  if (!*s) return;
  putstr_r(s + 1);
  putchar(*s);
}

void fizzbuzz(int i) {
  int printed = 0;

  if (i)
    fizzbuzz(i - 1);

  if (!(i % 3) && ++printed)
    putstr_r(\"zziF\");
  if (!(i % 5) && ++printed)
    putstr_r(\"zzuB\");

  if (!printed) {
    itoa_r(i, buff);
    putstr_r(buff);
  }

  putchar('\\\\n');
}

int main() {
  fizzbuzz(20);
  return 0;
}
" > $TEMP.c

gcc $TEMP.c -o $TEMP.gcc.out
$TEMP.gcc.out > $TEMP.gcc.txt

$AZCC $TEMP.c > $TEMP.azcc.s;

gcc $TEMP.azcc.s -o $TEMP.azcc.out
$TEMP.azcc.out > $TEMP.azcc.txt

cat $TEMP.azcc.txt

set +e

echo -n "\033[31m"
diff $TEMP.gcc.txt $TEMP.azcc.txt
DIFF=$?
echo -n "\033[m"

if [ $DIFF -eq 0 ]; then
  echo "\033[32mPASS\033[m"
  exit 0
else
  echo "\033[31mFAIL\033[m"
  exit 1
fi

