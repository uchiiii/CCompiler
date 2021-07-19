#include "9cc.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  user_input = argv[1];
  // tokenize
  tokenize();

  // token to AST
  program();

  printf(".intel_syntax noprefix\n");

  { // for the definition of global functions
    printf(".globl ");
    for (int i = 0; code[i]; i++) {
      if (i > 0) printf(", ");
      printf("%.*s", code[i]->len, code[i]->name);
    }
    printf("\n\n");
  } 


  for (int i = 0; code[i]; i++) {
    printf("%.*s:\n", code[i]->len, code[i]->name);

    // prologue
    // for variable
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %d\n", locals[0]->offset);
    
    gen(code[i]);
    printf("  pop rax\n"); // set on rax
    
    // epilogue
    // rax is still valid without dealing with it.
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n\n");
  }
  
  return 0;
}
