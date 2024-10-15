#include <stdint.h>
#include <stdio.h>

void foo(uint8_t argument) {
  uint8_t foo_local = 16;
  printf("Local variable address in foo: %p\n", &foo_local);
}

int main(void) {
  uint8_t main_local = 32;
  foo(main_local);
  printf("Local variable address in main: %p\n", &main_local);
}
