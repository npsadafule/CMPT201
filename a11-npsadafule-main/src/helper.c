#include "helper.h"
#include <stdio.h>

int convert(uint8_t *buff, ssize_t buff_size, char *str, ssize_t str_size) {
  if (buff == NULL || str == NULL || buff_size <= 0 ||
      str_size < (buff_size * 2 + 1)) {
    return -1;
  }

  for (int i = 0; i < buff_size; i++) {
    sprintf(str + i * 2, "%02X", buff[i]);
  }
  str[buff_size * 2] = '\0';

  return 0;
}