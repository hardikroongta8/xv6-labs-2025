#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

int main(int argc, char *argv[]) {
  int   size     = 8 * 4096;
  char *data     = sbrk(size);
  char *help_str = sbrk(32);

  strcpy(help_str, "This may help.");

  int i = 0;
  for (; i < size; i++) {
    if (strcmp(data + i, help_str) == 0) break;
  }
  if (i == size - 1) return 0;

  printf("%s\n", data + i + 16);
}
