#include <stdio.h>
#include <stdlib.h>

void free_memory(int *ptr) { free(ptr); }

int main() {
  int *ptr = (int *)malloc(sizeof(int));

  if (ptr == NULL) {
    perror("Memory allocatiopn failed");
    return 1;
  }

  *ptr = 42;

  free_memory(ptr);
  printf("Value after freeing: %d\n", *ptr);

  return 0;
}
