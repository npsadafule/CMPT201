#include <stdio.h>

struct align_example {
  char a;
  int b;
  short c;
  double d;
};

#pragma pack(push, 1)
struct pack_example {
  char a;
  int b;
  short c;
  double d;
};
#pragma pack(pop)

void align(void) {
  struct align_example ex = {'0', 0, 0, 0.1};

  printf("sizeof(char): %lu, sizeof(int): %lu, sizeof(short): %lu "
         "sizeof(double): %lu\n",
         sizeof(char), sizeof(int), sizeof(short), sizeof(double));
  printf("ex.a address: %p\nex.b address: %p\nex.c address: %p\nex.d address: "
         "%p\n",
         (void *)&ex.a, (void *)&ex.b, (void *)&ex.c, (void *)&ex.d);
  printf("sizeof(struct align_example): %lu\n", sizeof(struct align_example));
}

void pack(void) {
  struct pack_example ex = {'0', 0, 0, 0.1};
  printf("ex.a address: %p\nex.b address: %p\nex.c address: %p\nex.d address:"
         "%p\n",
         (void *)&ex.a, (void *)&ex.b, (void *)&ex.c, (void *)&ex.d);
  printf("sizeof(struct pack_example): %lu\n", sizeof(struct pack_example));
}

int main(void) {
  align();
  pack();
}
