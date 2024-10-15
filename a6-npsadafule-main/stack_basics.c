#include <stdint.h>
#include <stdio.h>

void test0(void) {
  int8_t local0 = 1;
  int8_t local_array[3] = {2, 3, 4};
  int8_t local1 = 5;

  printf("test0:\n");
  printf("  local0 address:         %p\n", &local0);
  printf("  local_array[0] address: %p\n", local_array);
  printf("  local_array[1] address: %p\n", (local_array + 1));
  printf("  local_array[2] address: %p\n", (local_array + 2));
  printf("  local1 address:         %p\n", &local1);
}

void test1(void) {
  int8_t local0 = 1;
  int8_t local_array[3] = {2, 3, 4};
  int8_t local1 = 5;

  printf("test1:\n");
  printf("  local0 value: %d\n", local0);

  local_array[3] = 24;

  printf("  local0 value: %d\n", local0);
}

void test2(void) {
  int8_t local_array[3] = {2, 3, 4};
  int8_t local0 = 5;

  printf("test2:\n");
  printf("  local_array[1] value: %d\n", local_array[1]);

  int8_t *ptr = &local0 + 2;
  *ptr = 36;

  printf(" local_array[1] value: %d\n", local_array[1]);
}

int main(void) {
  test0();
  test1();
  test2();
}
