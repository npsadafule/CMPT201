#include <limits.h>
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
