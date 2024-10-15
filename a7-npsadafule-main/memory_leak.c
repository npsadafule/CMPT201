#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_user_input() {
  size_t buffer_size = 100;
  char *buffer = (char *)malloc(buffer_size * sizeof(char));

  if (buffer == NULL) {
    perror("malloc() failed");
    return NULL;
  }

  printf("Enter your input: ");
  if (fgets(buffer, buffer_size, stdin) == NULL) {
    perror("fgets() failed");
    return NULL;
  }

  return buffer;
}

int main() {
  char *input = read_user_input();

  if (input != NULL) {
    printf("You entered: %s\n", input);
    free(input);
  }

  return 0;
}
