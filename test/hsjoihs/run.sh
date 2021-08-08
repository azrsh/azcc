#!/bin/sh

if [ $# -lt 2 ]; then
    echo "invalid arguments: $*"
    echo "required argument: generation of test target compiler(1st argument)"
    echo "required argument: path to test target compiler(2nd argument)"
    echo "optional argument: linking option(3rd argument)"
    exit 1
fi

GEN="$1"

CC=$(command -v "$2")
CC=$(readlink -f "$CC")

LDFLAGS="$3"

cd "$(dirname "$0")" || exit
make all "GEN=$GEN" "AZCC=$CC" "LDFLAGS=$LDFLAGS"
