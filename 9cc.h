#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

// input 
extern char *user_input;

// tokenize
typedef enum {
  TK_RESERVED,
  TK_IDENT,
  TK_NUM,
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
  ND_ASSIGN,
} NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;
  int offset;
};  
 
extern Node *code[100];

Node *expr(); 
void program();

// assembly code generation
void gen(Node *node);