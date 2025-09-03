#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

void find(char *, char *);

int main(int argc, char **argv) {
  if (argc < 3) {
    fprintf(2, "find: insufficient arguments\n");
  }

  char *dirpath = argv[1];
  char *filename = argv[2];

  find(dirpath, filename);
  exit(0);
}

void find(char *dirpath, char *filename) {
  int fd;
  struct stat st;
  struct dirent dnt;
  char pathbuf[512], *ptr;

  fd = open(dirpath, O_RDONLY);
  if (fd < 0) {
    fprintf(2, "find: cannot open %s\n", dirpath);
    exit(-1);
  }
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", dirpath);
    close(fd);
    exit(-1);
  }

  if (st.type != T_DIR) {
    fprintf(2, "find: %s must be a directory\n", dirpath);
    close(fd);
    exit(-1);
  }

  if (strlen(dirpath) + 1 + DIRSIZ + 1 > sizeof(pathbuf)) {
    fprintf(2, "find: directory path too long\n");
    close(fd);
    exit(-1);
  }

  strcpy(pathbuf, dirpath);
  ptr = pathbuf + strlen(pathbuf);
  *ptr++ = '/';

  while (read(fd, &dnt, sizeof(dnt)) == sizeof(dnt)) {
    if (dnt.inum == 0 || strcmp(dnt.name, ".") == 0 ||
        strcmp(dnt.name, "..") == 0) {
      continue;
    }

    memmove(ptr, dnt.name, DIRSIZ);
    ptr[DIRSIZ] = '\0';
    if (stat(pathbuf, &st) < 0) {
      fprintf(2, "find: cannot stat %s\n", pathbuf);
      close(fd);
      exit(-1);
    }
    if (st.type == T_DIR) {
      find(pathbuf, filename);
      continue;
    }
    if (st.type == T_FILE && strcmp(filename, dnt.name) == 0) {
      fprintf(1, "%s\n", pathbuf);
    }
  }

  close(fd);
}