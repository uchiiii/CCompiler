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

assert 0 'main() {return 0;}'
assert 42 'main() {return 42;}'
assert 21 'main() {return 5+20-4;}'
assert 41 'main() {return 12 + 34 -  5 ;}'
assert 47 'main() {return 5+6*7;}'
assert 15 'main() {return 5*(9-6);}'
assert 4 'main() {return (3+5)/2;}'
assert 16 'main() { return (3 + (1 +5)- 1*1) * 2;}'
assert 3 'main() {return - -3;}'
assert 15 'main() {return -3*-5;}'

assert 0 'main() {return 0==1;}'
assert 1 'main() {return 42==42;}'
assert 1 'main() {return 0!=1;}'
assert 0 'main() {return 42!=42;}'

assert 1 'main() {return 0<1;}'
assert 0 'main() {return 1<1;}'
assert 0 'main() {return 2<1;}'
assert 1 'main() {return 0<=1;}'
assert 1 'main() {return 1<=1;}'
assert 0 'main() {return 2<=1;}'

assert 1 'main() {return 1>0;}'
assert 0 'main() {return 1>1;}'
assert 0 'main() {return 1>2;}'
assert 1 'main() {return 1>=0;}'
assert 1 'main() {return 1>=1;}'
assert 0 'main() {return 1>=2;}'

assert 42 'main() {a=42; return a;}'
assert 42 'main() {return a = 42;}'
assert 21 'main() {b = 5+20-4; return b;}'
assert 21 'main() {return b = 5+20-4;}'
assert 41 'main() {c= 12 + 34 -  5 ; return c;}'
assert 47 'main() {d=5+6*7; return d;}'
assert 26 'main() {a=1;b=2;c=3;d=4;e=5;z=26; return z;}'
assert 26 'main() {a=1;b=2;c=3;d=4;e=5;return z=26;}'

assert 10 'main() {return cnt = 10;}'
assert 15 'main() {return num = 3 * 5;}'
assert 20 'main() {3+3/1; return res = 40 / 2;}'
assert 22 'main() {3+3/1; res = 40 / 2; return res = res + 2;}'
assert 10 'main() {res = 2; return res = res * 5;}'

assert 1  'main() {return 1;}'
assert 10 'main() {return 5 * 2;}'
assert 14 'main() {a = 3;b = 5 * 6 - 8;return a + b / 2;}'

assert 3  'main() {if(1) return 3;}'
assert 0  'main() {if(0) return 3; return 0;}'
assert 3  'main() {if(1 > 0) return 3; return 0;}'
assert 0  'main() {if(4 > 5) return 3; return 0;}'
assert 5  'main() {if(0) return 3; if(1) return 5; return 4;}'
assert 3  'main() {if(0) return 5; else return 3;}'
assert 5  'main() {if(1) return 5; else return 3;}'

assert 3  'main() { while(1) return 3;}'
assert 0  'main() { while(0) return 3; return 0;}'
assert 3  'main() { while(5 > 4) return 3; return 0;}'
assert 0  'main() { while(4 > 5) return 3; return 0;}'
assert 5  'main() { while(0) return 3; while(1) return 5; return 3;}'
assert 10 'main() { a = 0; while(a < 10) { a = a + 2;} return a;}'

assert 3  'main() { for(;;) return 3; return 5;}'
assert 3  'main() { for(0;;) return 3; return 5;}'
assert 5  'main() { for(;0;) return 3; return 5;}'
assert 0  'main() { j = 5; for(i = 0; i < 5; i = i+1) j = j - 1; return j;}'  

assert 0  'main() { {} return 0; }'
assert 1  'main() { { return 1;} return 0;}'
assert 1  'main() { a = 0; {3; a = a+1;} return a; }'
assert 4  'main() { if (1) { return 4;} else return 0; }'
assert 10 'main() { a = 0; for(i = 0; i < 10; i = i+1) { a = a + 1; } return a;}'
assert 10 'main() { a = 0; b = 0; for(i = 0; i < 10; i = i+1) { a = a + 1; b = b + 2;} return a;}'
assert 20 'main() { a = 0; b = 0; for(i = 0; i < 10; i = i+1) { a = a + 1; b = b + 2;} return b;}'

assert 2  'ident(a) { return a; }
main() { return ident(2); }'
assert 4  'ident(a) { return a; }
main() { return ident(1) + ident(3); }'
assert 3  'add(a, b) { return a+b; } 
main() { return add(1, 2); }'
assert 13  'linear(a, x, b) { return a*x+b; } 
main() { return linear(2, 5, 3); }'
assert 6 \
'sum(x) { if (x == 1) return 1; return sum(x-1) + x; }
main() {return sum(3);}'
assert 1 \
'fib(x) { if (x == 0) return 1; if (x == 1) return 1; return fib(x - 1) + fib(x - 2); }
main() { return fib(0); }'
assert 1 \
'fib(x) { if (x == 0) return 1; if (x == 1) return 1; return fib(x - 1) + fib(x - 2); }
main() { return fib(1); }'
assert 5 \
'fib(x) { if (x == 0) return 1; if (x == 1) return 1; return fib(x - 1) + fib(x - 2); }
main() { return fib(4); }'

echo OK
