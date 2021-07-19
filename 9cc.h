#ifndef _9CC_H_INCLUDED
#define _9CC_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

#define MX_NFUNC 100

// input 
extern char *user_input;

// tokenize
typedef enum {
  TK_RESERVED,
  TK_IDENT,
  TK_NUM,
  TK_RETURN,
  TK_IF,
  TK_ELSE,
  TK_WHILE,
  TK_FOR,
  TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token {
  TokenKind kind;
  Token *next;
  int val;
  char *str;
  int len;
};

typedef struct LVar LVar;

struct LVar {
  LVar *next;
  char *name;
  int len;
  int offset;
};

void tokenize();

extern Token *token;
extern char *user_input;

// construct AST
typedef enum {
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_NUM,
  ND_EQ,  // ==
  ND_NE,  // !=
  ND_LT,  // <
  ND_LE,  // <=
  ND_LVAR,
  ND_FUNCVAR,
  ND_FUNCDEF,
  ND_ASSIGN,
  ND_RETURN,
  ND_IF,
  ND_WHILE,
  ND_FOR,
  ND_BLOCK,
} NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind;
  Node *lhs; // for args in FUNCDEF
  Node *rhs; 
  Node *next; // for stmtlist in BLOCK and FUNCDEF and arguments in FUNCVAR
  char *name; // for FUNCVAR
  int len; // for FUNCVAR
  int val; // This is used as an index for if, while and for.
  int offset;
};  

extern Node *code[MX_NFUNC];
extern LVar *locals[MX_NFUNC];


Node *stmt();
Node *expr(); 
void program();

// assembly code generation
void gen(Node *node);

#endif
