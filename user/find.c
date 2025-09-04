#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

void find(const char *, const char *, int, char **);

int main(int argc, char **argv) {
  if (argc < 3) {
    fprintf(2, "find: insufficient arguments\n");
  }

  char *dirpath = argv[1];
  char *filename = argv[2];

  if (argc == 3) {
    find(dirpath, filename, 0, 0);
    exit(0);
  }
  const char *exec_str = "-exec";
  if (strcmp(argv[3], exec_str) != 0) {
    fprintf(2, "find: unrecognized option '%s'\n", argv[3]);
    exit(-1);
  }
  find(dirpath, filename, argc - 4, argv + 4);
  exit(0);
}

void find(const char *dirpath, const char *filename, int argc, char **argv) {
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
    ptr[DIRSIZ] = 0;
    if (stat(pathbuf, &st) < 0) {
      fprintf(2, "find: cannot stat %s\n", pathbuf);
      close(fd);
      exit(-1);
    }

    if (st.type == T_DEVICE) continue;

    if (st.type == T_DIR) {
      find(pathbuf, filename, argc, argv);
      continue;
    }

    // st.type == T_FILE
    if (strcmp(filename, dnt.name) != 0) continue;

    if (argc == 0) {
      fprintf(1, "%s\n", pathbuf);
      continue;
    }

    int pid = fork();
    if (pid < 0) {
      fprintf(2, "exec: fork error\n");
      close(fd);
      exit(-1);
    }

    // child process
    if (pid == 0) {
      close(fd);
      char *nargv[MAXARG];
      memset(nargv, 0, sizeof(nargv));
      for (int i = 0; i < argc; i++) {
        nargv[i] = argv[i];
      }
      nargv[argc] = pathbuf;
      exit(exec(argv[0], nargv));
    }

    // parent process
    if (pid > 0) {
      int exit_status = -1;
      if (wait(&exit_status) != pid) {
        fprintf(2, "wait: no child processes\n");
        close(fd);
        exit(-1);
      }

      if (exit_status != 0) {
        fprintf(2, "exec: error while executing '%s'\n", argv[0]);
        close(fd);
        exit(-1);
      }
    }
  }

  close(fd);
}