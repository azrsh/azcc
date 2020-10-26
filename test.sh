#!/bin/bash
assert() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5 ;"
assert 47 "5+6*7;"
assert 15 "5*(9-6);"
assert 4 "(3+5)/2;"
assert 10 "-10+20;"
assert 2 "-10/-5;"
assert 1 "10 == 10;"
assert 0 "10 < 10;"
assert 1 "20-10*1+2 <= 2*7;"
assert 0 "80-50 > 20*2;"
assert 1 "80-50 < 20*2;"
assert 0 "80-0 <= 20*2;"
assert 16 "a = 5*(9-6);b=a+1;"
assert 44 "a=29;b=5*(9-6);a+b;"
assert 25 "a=5*(9-6);b=26-3+2;"
assert 44 "asaskjkas=29;bakjehjioaj=5*(9-6);asaskjkas+bakjehjioaj;"
assert 40 "a=5*(9-6);b=26-3+2;return a + b;"
assert 15 "return 5*(9-6);b=26-3+2;"

echo OK

