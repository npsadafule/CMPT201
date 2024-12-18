#include <assert.h>
#include <limits.h>
#include <stdint.h>

int32_t absolute_value(int32_t num) {
  assert(num != INT32_MIN);
  int32_t ret = 0;

  if (num < 0) {
    ret = -num;
  } else {
    ret = num;
  }

  assert(ret >= 0);
  return ret;
}
