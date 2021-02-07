#!/bin/sh

if [ $# -lt 1 ]; then
    echo "invalid arguments: $*"
    echo "required argument: path to target compiler(1st argument)"
    echo "optional argument: linking option(2nd argument)"
    exit 1
fi


CC=`which $1`
CC=`readlink -f $CC`

LDFLAGS=$2

cd `dirname $0`
make test "AZCC=$CC" "LDFLAGS=$LDFLAGS" -j32
