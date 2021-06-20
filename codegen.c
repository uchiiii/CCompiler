#include <assert.h>
#include "9cc.h"

const char *regi[] = {
  "rdi", "rsi", "rdx", 
  "rcx", "r8", "r9",
};

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// AST to assembly
void gen_lval(Node *node) {
  if (node->kind != ND_LVAR) {
    error("代入の左辺値が変数ではありません");
  }
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void align() {
  // todo (align rsp to 0 (mod 16))
}

void gen(Node *node) {
  if(node == NULL) return;
  switch (node->kind) {
  case ND_NUM:
    printf("  push %d\n", node->val);
    return;
  case ND_FUNCVAR:
    {
      Node *cur = node;
      int var_len = 0;
      while(cur->next) {
        assert(var_len < 6);
        gen(cur->next);
        printf("  pop %s\n", regi[var_len++]);
        cur = cur->next;
      }
      printf("  call %.*s\n", node->len, node->name);
      printf("  push rax\n");
      return;
    }
  case ND_LVAR:
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case ND_ASSIGN:
    gen_lval(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    return;
  case ND_RETURN:
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  case ND_IF:
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lelse_IF_%d\n", node->val);
    gen(node->rhs->lhs);
    printf("  jmp .Lend_IF_%d\n", node->val);
    printf(".Lelse_IF_%d:\n", node->val);
    gen(node->rhs->rhs);
    printf(".Lend_IF_%d:\n", node->val);
    return;
  case ND_WHILE:
    printf(".Lbegin_WHILE_%d:\n", node->val);
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend_WHILE_%d\n", node->val);
    gen(node->rhs);
    printf("  jmp .Lbegin_WHILE_%d\n", node->val);
    printf(".Lend_WHILE_%d:\n", node->val);
    return;
  case ND_FOR:
    gen(node->lhs->lhs);
    printf(".Lbegin_FOR_%d:\n", node->val);
    printf("  push 1\n");
    gen(node->lhs->rhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend_FOR_%d\n", node->val);
    gen(node->rhs->lhs);
    gen(node->rhs->rhs);
    printf("  jmp .Lbegin_FOR_%d\n", node->val);
    printf(".Lend_FOR_%d:\n", node->val);
    return;
  case ND_BLOCK:
    while(node->next != NULL) {
      gen(node->next);  
      printf("  pop rax\n");
      node = node->next;
    }
    printf("  push 1\n"); // this is dummy for "pop rax" in 9cc.h
    return;
  }
  
  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch(node->kind) {
  case ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case ND_EQ:
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_NE:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LT:
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LE:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  }

  printf("  push rax\n");
}
