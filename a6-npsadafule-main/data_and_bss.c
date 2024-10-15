#include <stdio.h>

char bss_char0;
char bss_char1;
char data_char0 = '0';
char data_char1 = '0';

int main() {
  printf("data_char0 address: %p\n", &data_char0);
  printf("data_char1 address: %p\n", &data_char1);
  printf("bss_char0 address: %p\n", &bss_char0);
  printf("bss_char1 address: %p\n", &bss_char1);

  return 0;
}
