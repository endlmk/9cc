#!/bin/bash
try() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    gcc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

try 0 "0;"
try 42 "42;"
try 41 " 12 + 34 - 5;"
try 47 '5+6*7;'
try 15 '5*(9-6);'
try 4 '(3+5)/2;'
try 10 '-10 + 20;'
try 1 '3 == 3;'
try 0 '4!= 4;'
try 1 '(2*3) < 7;'
try 0 '(2+4) <= 5;'
try 1 '-3 > -5;'
try 0 '-2 > 4;'
try 6 'a = 3; a * 2;'
try 3 'a = 3; b = a * 2; c = b / a + 1; c;'
try 5 'return 5;'
try 4 'a = 3; b = a * 2; return b / a + 2;'
echo OK