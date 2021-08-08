#!/bin/sh

# 第一引数として、コンパイラへのパスを取る

if [ $# -lt 2 ]; then
    echo "invalid arguments: $*"
    echo "required argument: generation of test target compiler(1st argument)"
    echo "required argument: path to target compiler(2nd argument)"
    echo "optional argument: linking option(3rd argument)"
    exit 1
fi

GEN=$1
AZCC=$2
LDFLAGS=${3-"-static"}
TEMP=$(mktemp)
SRC=$(dirname "$0")/fizzbuzz.c

set -e

gcc "$SRC" -S -o "$TEMP".gcc.s
as "$TEMP".gcc.s -o "$TEMP".gcc.o
gcc "$TEMP".gcc.o -o "$TEMP".gcc.out "$LDFLAGS"
"$TEMP".gcc.out > "$TEMP".gcc.txt

"$AZCC" "$SRC" > "$TEMP".azcc.s
as "$TEMP".azcc.s -o "$TEMP".azcc.o
gcc "$TEMP".azcc.o -o "$TEMP".azcc.out "$LDFLAGS"
"$TEMP".azcc.out > "$TEMP".azcc.txt

cat "$TEMP".azcc.txt

set +e

printf "\033[31m"
diff "$TEMP".gcc.txt "$TEMP".azcc.txt
DIFF="$?"
printf "\033[m"

rm "$TEMP"*

if [ "$DIFF" -eq 0 ]; then
  echo "\033[32mPASS\033[m"
  exit 0
else
  echo "\033[31mFAIL\033[m"
  exit 1
fi

