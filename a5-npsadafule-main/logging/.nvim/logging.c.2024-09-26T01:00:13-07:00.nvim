#include "log.h"
#include <stdio.h>

int main() {
  log_set_level(LOG_TRACE);
  log_trace("Program started.");

  int size = 5;
  int numbers[size];
  int sum = 0;

  for (int i = 0; i < size; ++i) {
    log_debug("numbers[%d] = %d", i, i);
    numbers[i] = i;
    sum += numbers[i];
  }

  log_info("Sum calculated: %d", sum);
  printf("The sum is: %d\n", sum);

  log_trace("Program ended");
  return 0;
}
