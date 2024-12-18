#include <stddef.h>
#include <stdint.h>

int32_t absolute_value(int32_t num) {
  int32_t ret = 0;

  if (num < 0) {
    ret = -num;
  } else {
    ret = num;
  }

  return ret;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  uint8_t input[sizeof(int32_t)] = {0};

  size_t new_size = (size > sizeof(int32_t)) ? sizeof(int32_t) : size;
  for (size_t i = 0; i < new_size; ++i) {
    input[i] = data[i];
  }

  absolute_value(*(int32_t *)input);

  return 0;
}
