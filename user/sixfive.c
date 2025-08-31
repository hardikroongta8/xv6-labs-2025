#include "kernel/fcntl.h"
#include "kernel/types.h"
#include "user/user.h"

char get_char(int fd) {
  char buf[1];
  int n = read(fd, buf, 1);
  if (n < 0) {
    fprintf(2, "sixfive: error while reading the file\n");
    exit(1);
  }
  if (n == 0) {
    return 0;
  }
  return buf[0];
}

void print_num(int num) {
  int n = num;
  int size = 0;
  while (n) {
    size++;
    n = n / 10;
  }
  char number[size + 1];
  for (int i = 0; i < size; i++) {
    number[size - i - 1] = '0' + (num % 10);
    num = num / 10;
  }
  for (int i = 0; i < size; i++) {
    char ch[1] = {number[i]};
    write(1, ch, 1);
  }
  fprintf(1, "\n");
}

void sixfive(int fd) {
  const char *separators = "-\r\t\n./,";
  const char *nums = "0123456789";

  while (1) {
    char c = get_char(fd);
    if (c == 0) {
      return;
    }

    // neither separator nor number
    if (strchr(separators, c) == 0 && strchr(nums, c) == 0) {
      while (1) {
        char c1 = get_char(fd);
        if (c1 == 0) {
          return;
        }
        if (strchr(separators, c1)) {
          break;
        }
      }
      continue;
    }

    if (strchr(separators, c)) {
      continue;
    }

    int num = c - '0';
    int is_num = 1;

    char c1;
    while (1) {
      c1 = get_char(fd);
      if (c1 == 0) {
        break;
      }
      if (strchr(nums, c1)) {
        num = (num * 10) + (c1 - '0');
        continue;
      }
      if (strchr(separators, c1)) {
        break;
      }
      is_num = 0;
      break;
    }

    if (is_num) {
      if (num % 5 == 0 || num % 6 == 0) {
        print_num(num);
      }
      continue;
    }

    if (c1 == 0) {
      return;
    }

    while (1) {
      c1 = get_char(fd);
      if (c1 == 0) {
        return;
      }
      if (strchr(separators, c1)) {
        break;
      }
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(2, "sixfive: insufficient arguments\n");
    exit(1);
  }

  for (int i = 0; i < argc; i++) {
    int fd = open(argv[i], O_RDONLY);
    if (fd < 0) {
      fprintf(2, "sixfive: error opening the file\n");
      exit(1);
    }

    sixfive(fd);
    close(fd);
  }
  exit(0);
}