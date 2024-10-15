#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int random_range(int min, int max) {
  if (min >= max) {
    fprintf(stderr, "Error: Invalid range.\n");
    exit(1);
  }

  srand(time(NULL));
  return (rand() % (max - min + 1)) + min;
}
