#include <stdint.h>
#include <stdio.h>

int main(void) {
  int (*main_ptr)(void);
  main_ptr = &main;

  uint8_t *start_address = (uint8_t *)main_ptr;

  printf("Dumping memory from address %p:\n", start_address);

  for (size_t i = 0; i < 64; i++) {
    printf("%02X ", start_address[i]);

    if ((i + 1) % 16 == 0)
      printf("\n");
  }

  return 0;
}
