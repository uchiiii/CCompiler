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

assert 0 '0;'
assert 42 '42;'
assert 21 '5+20-4;'
assert 41 ' 12 + 34 -  5 ;'
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 16 '(3 + (1 +5)- 1*1) * 2;'
assert 3 '- -3;'
assert 15 '-3*-5;'

assert 0 '0==1;'
assert 1 '42==42;'
assert 1 '0!=1;'
assert 0 '42!=42;'

assert 1 '0<1;'
assert 0 '1<1;'
assert 0 '2<1;'
assert 1 '0<=1;'
assert 1 '1<=1;'
assert 0 '2<=1;'

assert 1 '1>0;'
assert 0 '1>1;'
assert 0 '1>2;'
assert 1 '1>=0;'
assert 1 '1>=1;'
assert 0 '1>=2;'

assert 42 'a=42;'
assert 21 'b = 5+20-4;'
assert 41 'c= 12 + 34 -  5 ;'
assert 47 'd=5+6*7;'
assert 26 'a=1;b=2;c=3;d=4;e=5;z=26;'

assert 10 'cnt = 10;'
assert 15 'num = 3 * 5;'
assert 20 '3+3/1; res = 40 / 2;'
assert 22 '3+3/1; res = 40 / 2; res = res + 2;'
assert 10 'res = 2; res = res * 5;'

assert 1  'return 1;'
assert 10 'return 5 * 2;'
assert 14 'a = 3;b = 5 * 6 - 8;return a + b / 2;'

assert 3  'if(1) return 3;'
assert 0  'if(0) return 3; 0;'
assert 3  'if(1 > 0) return 3; 0;'
assert 0  'if(4 > 5) return 3; 0;'
assert 5  'if(0) return 3; if(1) return 5; 4;'
assert 3  'if(0) return 5; else return 3;'
assert 5  'if(1) return 5; else return 3;'

assert 3  'while(1) return 3;'
assert 0  'while(0) return 3; 0;'
assert 3  'while(5 > 4) return 3; 0;'
assert 0  'while(4 > 5) return 3; 0;'
assert 5  'while(0) return 3; while(1) return 5; 3;'

assert 3  'for(;;) return 3; 5;'
assert 3  'for(0;;) return 3; 5;'
assert 5  'for(;0;) return 3; 5;'
assert 0  'j = 5; for(i = 0; i < 5; i = i+1) j = j - 1; j;'  

assert 0  '{} 0;'
assert 1  'a = 0; {3; a = a+1;} a;'
assert 4  'if (1) { return 4;} else return 0;'
assert 10 'a = 0; for(i = 0; i < 10; i = i+1) { a = a + 1; } a;'
assert 10 'a = 0; b = 0; for(i = 0; i < 10; i = i+1) { a = a + 1; b = b + 2;} a;'
assert 20 'a = 0; b = 0; for(i = 0; i < 10; i = i+1) { a = a + 1; b = b + 2;} b;'

echo OK
