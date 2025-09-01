#include "kernel/fcntl.h"
#include "kernel/types.h"
#include "user/user.h"

void memdump(char *fmt, char *data);
void handle_p(char *data);
void handle_i(char *data);
void handle_h(char *data);
void handle_c(char *data);
void handle_s(char *data);
void handle_S(char *data);

int main(int argc, char *argv[]) {
  if (argc == 1) {
    printf("Example 1:\n");
    int a[2] = {61810, 2025};
    memdump("ii", (char *)a);

    printf("Example 2:\n");
    memdump("S", "a string");

    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *)&s);

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;

    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");

    printf("Example 4:\n");
    memdump("pihcS", (char *)&example);

    printf("Example 5:\n");
    memdump("sccccc", (char *)&example);
  } else if (argc == 2) {
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while (n < sizeof(data)) {
      int nn = read(0, data + n, sizeof(data) - n);
      if (nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void memdump(char *fmt, char *data) {
  int arg_len = 0;
  while (fmt[arg_len] != '\0') {
    arg_len++;
  }
  int bytes_read = 0;
  for (int i = 0; i < arg_len; i++) {
    char fc = fmt[i];
    switch (fc) {
    case 'i':
      handle_i(data + bytes_read);
      bytes_read += 4;
      break;
    case 'p':
      handle_p(data + bytes_read);
      bytes_read += 8;
      break;
    case 'h':
      handle_h(data + bytes_read);
      bytes_read += 2;
      break;
    case 'c':
      handle_c(data + bytes_read);
      bytes_read += 1;
      break;
    case 's':
      handle_s(data + bytes_read);
      bytes_read += 8;
      break;
    case 'S':
      handle_S(data + bytes_read);
      return;
    default:
      fprintf(2, "memdump: unknown character in format string");
      exit(1);
    }
  }
}

void handle_i(char *data) { printf("%d\n", *(unsigned int *)data); }

void handle_p(char *data) { printf("%lx\n", *(unsigned long *)data); }

void handle_h(char *data) { printf("%d\n", *(unsigned short *)data); }

void handle_c(char *data) { printf("%c\n", *data); }

void handle_s(char *data) { printf("%s\n", *(char **)data); }

void handle_S(char *data) { printf("%s\n", data); }