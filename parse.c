#include "9cc.h"

Token *token;
char *user_input;
int n_func = 0;

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;

  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, " ");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

bool consume(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
    return false;

  token = token->next;
  return true;
}

bool consume_type() {
  if (token->kind != TK_TYPE) return false;
  token = token->next;
  return true;
}

bool consume_if() {
  if (token->kind != TK_IF) return false;

  token = token->next;
  return true;
}

bool consume_else() {
  if (token->kind != TK_ELSE) return false;

  token = token->next;
  return true;
}

bool consume_while() {
  if (token->kind != TK_WHILE) return false;

  token = token->next;
  return true;
}

bool consume_for() {
  if (token->kind != TK_FOR) return false;

  token = token->next;
  return true;
}

bool consume_return() {
  if (token->kind != TK_RETURN) return false;
  
  token = token->next;
  return true; 
}

Token *consume_ident() {
  if (token->kind != TK_IDENT) {
    return NULL;
  }
  
  Token *ident = token;
  token = token->next;
  return ident;
}

/*
 * check_hoge does not consume token,
 * but just return token.
 */
Token *check_ident() {
  if (token->kind != TK_IDENT) {
    return NULL;
  }
  
  Token *ident = token;
  return ident;
}

void expect(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
    error_at(token->str, "expected \"%s\"", op);

  token = token->next;
}

int expect_number() {
  if (token->kind != TK_NUM) {
    error_at(token->str, "数ではありません");
  }
  int val = token->val;
  token = token->next;
  return val;
}

void expect_type() {
  if (token->kind != TK_TYPE) {
    error_at(token->str, "型ではありません");
  }
  token = token->next;
}

bool next_is(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
    return false;
  else return true;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

bool startswith(char *p, char *q) {
  return memcmp(p, q, strlen(q)) == 0;
}

int is_alnum(char c) {
  return ('a' <= c && c <= 'z') ||
         ('A' <= c && c <= 'Z') ||
         ('0' <= c && c <= '9') ||
         (c == '_');
}

int search_lvar(char *p) {
  char *cur = p;
  while (cur && is_alnum(*cur)) {
    cur++;
  }
  return cur - p;
}

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok -> kind = kind;
  tok -> str = str;
  tok -> len = len;
  cur -> next = tok;
  return tok;
}

void tokenize() {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  char *p = user_input;

  while(*p) {
    if(isspace(*p)) {
      p++;
      continue;
    }
    if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") || startswith(p, ">=")) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }
    
    if (strchr("+-*/()<>;={},&", *p)) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 0);
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - q;
      continue;
    }

    if (startswith(p, "if") && !is_alnum(p[2])) {
      cur = new_token(TK_IF, cur, p, 2);
      p += 2;
      continue;
    }

    if (startswith(p, "else") && !is_alnum(p[4])) {
      cur = new_token(TK_ELSE, cur, p, 4);
      p += 4;
      continue;
    }

    if (startswith(p, "while") && !is_alnum(p[5])) {
      cur = new_token(TK_WHILE, cur, p, 5);
      p += 5;
      continue;
    }

    if (startswith(p, "for") && !is_alnum(p[3])) {
      cur = new_token(TK_FOR, cur, p, 3);
      p += 3;
      continue;
    }

    if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
      cur = new_token(TK_RETURN, cur, p, 6);
      p += 6;
      continue;
    }

    if (strncmp(p, "int", 3) == 0 && !is_alnum(p[3])) {
      cur = new_token(TK_TYPE, cur, p, 3);
      p += 3;
      continue;
    } 
    
    int len = search_lvar(p);
    if (len) {
      cur = new_token(TK_IDENT, cur, p, len);
      p += len;
      continue;
    }

    error_at(p, "invalid token");
  }

  new_token(TK_EOF, cur, p, 0);
  
  token = head.next;
}

// construct AST
Node *code[MX_NFUNC];
LVar *locals[MX_NFUNC];
LVar *local_vars; // local variables for current functions
int n_labels;

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node -> kind = kind;
  node -> lhs = lhs;
  node -> rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node -> kind = ND_NUM;
  node -> val = val;
  return node;
}

LVar *find_lvar(Token *tok) {
  for (LVar *var = local_vars; var; var = var->next) {
    if(var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
      return var;
    }
  }
  return NULL;
}

Node *args() {
  if(next_is(")")) return NULL;
  Node *node = expr();
  Node *tail = node;
  for(;consume(",");) {
    tail->next = expr();
    tail = tail->next;
  }

  return node;
}

Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }


  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    
    if (consume("(")) { // FUNCVAR
      node->kind = ND_FUNCVAR;
      node->name = tok->str;
      node->len = tok->len;
      node->next = args();
      consume(")");
      return node;
    } else { // LVAR
      node->kind = ND_LVAR;
      LVar *lvar = find_lvar(tok);
      if (lvar) {
        node->offset = lvar->offset;
      } else {
        error_at(tok->str, "\"%.*s\" is not decleared", tok->len, tok->str);
      }
      return node;
    }
  }
  return new_node_num(expect_number());
}

Node *unary() {
  if (consume("+")) {
    return unary();
  }
  if (consume("-")) {
    return new_node(ND_SUB, new_node_num(0), unary());
  }
  if (consume("&")) {
    return new_node(ND_ADDR, primary(), NULL);
  }
  if (consume("*")) {
    return new_node(ND_DEREF, primary(), NULL); 
  }
  return primary();
}

Node *mul() {
  Node *node = unary();

  for(;;) {
    if (consume("*")) {
      node = new_node(ND_MUL, node, unary());
    } else if(consume("/")) {
      node = new_node(ND_DIV, node, unary());
    } else {
      return node;
    }
  }
}

Node *add() {
  Node *node = mul();
  
  for(;;) {
    if(consume("+")) {
      node = new_node(ND_ADD, node, mul());
    } else if(consume("-")) {
      node = new_node(ND_SUB, node, mul());
    } else {
      return node;
    }
  }
}

Node *relational() {
  Node *node = add();

  for(;;) {
    if(consume("<")) {
      node = new_node(ND_LT, node, add());
    } else if(consume("<=")) {
      node = new_node(ND_LE, node, add());
    } else if(consume(">")) {
      node = new_node(ND_LT, add(), node);
    } else if(consume(">=")) {
      node = new_node(ND_LE, add(), node);
    } else {
      return node;
    }
  }
}

Node *equality() {
  Node *node = relational();

  for(;;) {
    if(consume("==")) {
      node = new_node(ND_EQ, node, relational());
    } else if (consume("!=")) {
      node = new_node(ND_NE, node, relational());
    } else {
      return node;
    }
  }
}

Node *assign() {
  Node *node = equality();
  
  if (consume("=")) {
    node = new_node(ND_ASSIGN, node, assign());
  }
  return node;
}

Node *decl_var() {
  Token *tok = check_ident();

  // (TODO) avoid same name
  
  LVar *lvar = calloc(1, sizeof(LVar));
  lvar->next = local_vars;
  lvar->name = tok->str;
  lvar->len = tok->len;
  lvar->offset = local_vars->offset + 8; 
  local_vars = lvar;

  return assign();
}

Node *expr() {
  if(consume_type()) {
    return decl_var();
  }
  return assign();
}

Node *if_stmt() {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_IF;
  node->val = n_labels++;
  expect("(");
  node->lhs =  expr();
  expect(")");
  node->rhs = calloc(1, sizeof(Node));
  node->rhs->lhs = stmt();
  if (consume_else()) {
    node->rhs->rhs = stmt();
  }
  return node;
}

Node *while_stmt() {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_WHILE;
  node->val = n_labels++;
  expect("(");
  node->lhs = expr();
  expect(")");
  node->rhs = stmt();
  return node;
}

Node *for_stmt() {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_FOR;
  node->val = n_labels++;
  expect("(");
  node->lhs = calloc(1, sizeof(Node));
  node->rhs = calloc(1, sizeof(Node));
  if(consume(";")) {} else {
    node->lhs->lhs = expr();
    expect(";");
  }
  if(consume(";")) {} else {
    node->lhs->rhs = expr();
    expect(";");
  }
  if(consume(")")) {} else {
    node->rhs->rhs = expr();
    expect(")");
  }

  node->rhs->lhs = stmt();
  return node;
}

Node *block() {
  expect("{");
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_BLOCK;
  node->lhs = new_node_num(0); // dummy
  Node *tail = node->lhs;
  while(!consume("}")) {
    tail->next = stmt();
    tail = tail->next;
  }

  return node;
}

Node *stmt() {
  Node *node;
  
  if (consume_return()) { // RETURN STATEMENT
    node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->lhs = expr();
    expect(";");
  } else if(consume_if()) { // IF STATEMENT
    node = if_stmt();  
  } else if(consume_while()) { // WHILE STATEMENT
    node = while_stmt();
  } else if(consume_for()) { // FOR STATEMENT
    node = for_stmt(); 
  } else if(next_is("{")) { // BLOCK
    node = block();
  } else {
    node = expr();
    expect(";");
  }
  return node;
}

Node *def_func() {
  Node *node = calloc(1, sizeof(Node)); 
  node->kind = ND_FUNCDEF;
  expect_type();
  { // func name
    Token *tok = consume_ident();
    node->name = tok->str;
    node->len = tok->len;
  }
  { // args
    expect("(");
    node->lhs = args();
    expect(")");
  }

  node->next = block();
  return node;
}

void program() {

  while(!at_eof()) {
    // initialization with dummy 
    local_vars = calloc(1, sizeof(LVar)); // TODO: free previous local_vars
    local_vars->offset = 0;
    
    code[n_func] = def_func();
    locals[n_func] = local_vars;

    n_func += 1;
  }
  code[n_func] = NULL;
}
