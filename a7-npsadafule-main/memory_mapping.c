#include <stdio.h>

void foo(void) { printf("foo() address:      %p\n", foo); }

int main(void) {
  int stack_var = 0;

  printf("main() address:     %p\n", main);
  printf("printf() address:   %p\n", printf);
  printf("stack_var address:  %p\n", &stack_var);
  foo();
}
