#include <stdint.h>
#include <stdio.h>

struct as_struct {
  uint16_t l2b;
  uint8_t n1b;
  uint8_t h1b;
};

union as_union {
  uint16_t r2b;
  uint32_t r4b;
  uint8_t r1b;
};

int main(void) {
  uint32_t i = 0xBEEFCAFE;
  uint16_t *ptr16 = (uint16_t *)&i;
  struct as_struct *as = (struct as_struct *)&i;
  union as_union *au = (union as_union *)&i;

  printf("i: 0x%X\n", i);
  printf("ptr16[0]: 0x%hX, ptr16[1]: 0x%hX\n", ptr16[0], ptr16[1]);
  printf("lowest two bytes: 0x%hX, next one byte: 0x%hhX, highest one byte: "
         "0x%hhX\n",
         as->l2b, as->n1b, as->h1b);
  printf(
      "reading four bytes: 0x%X, reading two bytes: 0x%hX, reading one byte: "
      "0x%hhX\n",
      au->r4b, au->r2b, au->r1b);
}
