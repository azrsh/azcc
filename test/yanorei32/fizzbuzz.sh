#!/bin/sh

# 第一引数として、コンパイラへのパスを取る

if [ $# != 1 ]; then
    echo invalid arguments: $*
    echo required only path to target compiler.
    exit 1
fi

AZCC=$1
TEMP=$(mktemp)
SRC=`dirname $0`/fizzbuzz.c

set -e

gcc $SRC -o $TEMP.gcc.out
$TEMP.gcc.out > $TEMP.gcc.txt

$AZCC $SRC > $TEMP.azcc.s;

gcc $TEMP.azcc.s -o $TEMP.azcc.out -static
$TEMP.azcc.out > $TEMP.azcc.txt

cat $TEMP.azcc.txt

set +e

echo -n "\033[31m"
diff $TEMP.gcc.txt $TEMP.azcc.txt
DIFF=$?
echo -n "\033[m"

rm $TEMP*

if [ $DIFF -eq 0 ]; then
  echo "\033[32mPASS\033[m"
  exit 0
else
  echo "\033[31mFAIL\033[m"
  exit 1
fi

