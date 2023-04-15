extern int i;

void entrypoint(int dummy) {
  asm("movq $60, %%rax\n\t"
      "movl %0, %%edi\n\t"
      "syscall"
      :
      : "r"(i));
}
