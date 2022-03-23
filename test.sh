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

assert 0 'int main() {return 0;}'
assert 42 'int main() {return 42;}'
assert 21 'int main() {return 5+20-4;}'
assert 41 'int main() {return 12 + 34 -  5 ;}'
assert 47 'int main() {return 5+6*7;}'
assert 15 'int main() {return 5*(9-6);}'
assert 4 'int main() {return (3+5)/2;}'
assert 16 'int main() { return (3 + (1 +5)- 1*1) * 2;}'
assert 3 'int main() {return - -3;}'
assert 15 'int main() {return -3*-5;}'

assert 0 'int main() {return 0==1;}'
assert 1 'int main() {return 42==42;}'
assert 1 'int main() {return 0!=1;}'
assert 0 'int main() {return 42!=42;}'

assert 1 'int main() {return 0<1;}'
assert 0 'int main() {return 1<1;}'
assert 0 'int main() {return 2<1;}'
assert 1 'int main() {return 0<=1;}'
assert 1 'int main() {return 1<=1;}'
assert 0 'int main() {return 2<=1;}'

assert 1 'int main() {return 1>0;}'
assert 0 'int main() {return 1>1;}'
assert 0 'int main() {return 1>2;}'
assert 1 'int main() {return 1>=0;}'
assert 1 'int main() {return 1>=1;}'
assert 0 'int main() {return 1>=2;}'

assert 42 'int main() {int a; a=42; return a;}'
assert 42 'int main() {int a; return a = 42;}'
assert 21 'int main() {int b; b = 5+20-4; return b;}'
assert 21 'int main() {int b; return b = 5+20-4;}'
assert 41 'int main() {int c; c= 12 + 34 -  5 ; return c;}'
assert 47 'int main() {int d; d=5+6*7; return d;}'
assert 26 'int main() {int a;int b;int c;int d;int e;int z;a=1;b=2;c=3;d=4;e=5;z=26; return z;}'
assert 26 'int main() {int a;int b;int c;int d;int e;int z;a=1;b=2;c=3;d=4;e=5;return z=26;}'
assert 42 'int main() {int a = 42; return a;}'
assert 21 'int main() {int b = 5+20-4; return b;}'
assert 41 'int main() {int c = 12 + 34 -  5 ; return c;}'
assert 47 'int main() {int d=5+6*7; return d;}'
assert 26 'int main() {int a=1; int b=2; int c=3; int d=4; int e=5; int z=26; return z;}'

assert 10 'int main() {int cnt; return cnt = 10;}'
assert 15 'int main() {int num; return num = 3 * 5;}'
assert 20 'int main() {3+3/1; int res;  return res = 40 / 2;}'
assert 22 'int main() {3+3/1; int res;  res = 40 / 2; return res = res + 2;}'
assert 10 'int main() {int res; res = 2; return res = res * 5;}'

assert 1  'int main() {return 1;}'
assert 10 'int main() {return 5 * 2;}'
assert 14 'int main() {int a; int b; a = 3;b = 5 * 6 - 8;return a + b / 2;}'

assert 3  'int main() {if(1) return 3;}'
assert 0  'int main() {if(0) return 3; return 0;}'
assert 3  'int main() {if(1 > 0) return 3; return 0;}'
assert 0  'int main() {if(4 > 5) return 3; return 0;}'
assert 5  'int main() {if(0) return 3; if(1) return 5; return 4;}'
assert 3  'int main() {if(0) return 5; else return 3;}'
assert 5  'int main() {if(1) return 5; else return 3;}'

assert 3  'int main() { while(1) return 3;}'
assert 0  'int main() { while(0) return 3; return 0;}'
assert 3  'int main() { while(5 > 4) return 3; return 0;}'
assert 0  'int main() { while(4 > 5) return 3; return 0;}'
assert 5  'int main() { while(0) return 3; while(1) return 5; return 3;}'
assert 10 'int main() { int a; a = 0; while(a < 10) { a = a + 2;} return a;}'

assert 3  'int main() { for(;;) return 3; return 5;}'
assert 3  'int main() { for(0;;) return 3; return 5;}'
assert 5  'int main() { for(;0;) return 3; return 5;}'
assert 0  'int main() { int i; int j; j = 5; for(i = 0; i < 5; i = i+1) j = j - 1; return j;}'  

assert 0  'int main() { {} return 0; }'
assert 1  'int main() { { return 1;} return 0;}'
assert 1  'int main() { int a; a = 0; {3; a = a+1;} return a; }'
assert 4  'int main() { if (1) { return 4;} else return 0; }'
assert 10 'int main() { int a; int i; a = 0; for(i = 0; i < 10; i = i+1) { a = a + 1; } return a;}'
assert 10 'int main() { int a; int b; int i;  a = 0; b = 0; for(i = 0; i < 10; i = i+1) { a = a + 1; b = b + 2;} return a;}'
assert 20 'int main() { int a; int b; int i; a = 0; b = 0; for(i = 0; i < 10; i = i+1) { a = a + 1; b = b + 2;} return b;}'

assert 2  \
'int ident(int a) { return a; }
int main() { return ident(2); }'
assert 4  \
'int ident(int a) { return a; }
int main() { return ident(1) + ident(3); }'
assert 3  \
'int add(int a, int b) { return a+b; } 
int main() { return add(1, 2); }'
assert 13  \
'int linear(int a, int x, int b) { return a*x+b; } 
int main() { return linear(2, 5, 3); }'
assert 6 \
'int sum(int x) { if (x == 1) return 1; return sum(x-1) + x; }
int main() {return sum(3);}'
assert 1 \
'int fib(int x) { if (x == 0) return 1; if (x == 1) return 1; return fib(x - 1) + fib(x - 2); }
int main() { return fib(0); }'
assert 1 \
'int fib(int x) { if (x == 0) return 1; if (x == 1) return 1; return fib(x - 1) + fib(x - 2); }
int main() { return fib(1); }'
assert 5 \
'int fib(int x) { if (x == 0) return 1; if (x == 1) return 1; return fib(x - 1) + fib(x - 2); }
int main() { return fib(4); }'
 
assert 3 'int main() { int x; int y; x = 3; y = &x; return *y; }'
assert 3 \
'int main() { int x; int y; int z; x = 3; y = 5; z = &y + 8; return *z;  }'

echo OK
