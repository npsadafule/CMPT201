#include "abs.h"
#include <stdint.h>
#include <stdio.h>

int main() {
  int32_t test_value = INT32_MIN;
  printf("Calling absolute_value() with %d\n", test_value);
  int32_t result = absolute_value(test_value);
  printf("Absolute value: %d\n", result);
  return 0;
}
