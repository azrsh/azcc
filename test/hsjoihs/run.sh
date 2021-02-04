#!/bin/sh

CC=`which $1`
CC=`readlink -f $CC`
echo $CC

cd `dirname $0`
make test "AZCC=$CC" -j32
