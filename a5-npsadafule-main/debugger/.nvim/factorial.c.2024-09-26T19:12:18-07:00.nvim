#include <stdio.h>
#include <stdlib.h>

long factorial(int n);

int main(void) {
  char input_buffer[10];
  int n = 0;

  if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
    printf("Error reading input.\n");
    return 1;
  }

  n = atoi(input_buffer);

  long val = factorial(n);
  printf("%ld\n", val);
  return 0;
}

long factorial(int n) {
  long result = 1;
  while (n--) {
    result *= n;
  }
  return result;
}
