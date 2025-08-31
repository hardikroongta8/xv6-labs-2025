#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  if (argc < 2 || argc > 2) {
    printf("sleep: insufficient arguments\n");
  }

  int ticks = atoi(argv[1]);
  if (pause(ticks) < 0) {
    exit(1);
  }
  exit(0);
}