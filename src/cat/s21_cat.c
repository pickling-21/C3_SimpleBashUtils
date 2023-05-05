#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

typedef struct {
  int bflag;
  int eflag;
  int nflag;
  int sflag;
  int tflag;
  int vflag;
  int errorflag;
} flags;

int parser(int argc, char *argv[], flags *flag);
void cat(char *path, flags flag);

int main(int argc, char *argv[]) {
  int number_of_files;
  flags option = {0};
  number_of_files = parser(argc, argv, &option);

  if (option.errorflag == 1) {
    fprintf(stderr, "illegal option");
  } else {
    while (number_of_files < argc) {
      char *path;
      path = argv[number_of_files];
      cat(path, option);
      number_of_files++;
    }
  }
  return 0;
}

int parser(int argc, char *argv[], flags *flag) {
  const struct option long_options[] = {
      {"number-nonblank", no_argument, NULL, 'b'},
      {"number", no_argument, NULL, 'n'},
      {"squeeze-blank", no_argument, NULL, 's'},
      {NULL, 0, NULL, 0}};

  int opt = 0;

  while ((opt = getopt_long(argc, argv, "beEnstTv?", long_options, NULL)) !=
         -1) {
    switch (opt) {
      case 'b':
        flag->bflag = 1;
        break;
      case 'e':
        flag->eflag = flag->vflag = 1;
        break;
      case 'E':
        flag->eflag = 1;
        break;
      case 'n':
        flag->nflag = 1;
        break;
      case 's':
        flag->sflag = 1;
        break;
      case 't':
        flag->tflag = flag->vflag = 1;
        break;
      case 'T':
        flag->tflag = 1;
        break;
      case 'v':
        flag->vflag = 1;
        break;
      case '?':
        flag->errorflag = 1;
        break;
    }
  }
  return optind;
}

void cat(char *path, flags flag) {
  if (access(path, F_OK) == 0) {
    FILE *filename;
    filename = fopen(path, "r");

    char ch, prev;

    int current_line = 1;
    int squeeze = 0;
    for (prev = '\n'; (ch = getc(filename)) != EOF; prev = ch) {
      if (flag.sflag == 1) {
        if (ch == '\n' && prev == '\n') {
          if (squeeze == 1) {
            continue;
          }
          squeeze++;
        } else
          squeeze = 0;
      }

      if (flag.bflag == 1) {
        if (prev == '\n' && ch != '\n') {
          printf("%*d\t", 6, current_line);
          current_line++;
        }
      }

      if (flag.nflag == 1 && flag.bflag == 0 && prev == '\n') {
        printf("%*d\t", 6, current_line);
        current_line++;
      }

      if (flag.eflag == 1 && ch == '\n') {
        putchar('$');
      }

      if (flag.tflag == 1 && ch == '\t') {
        printf("^");
        ch = 'I';
      }

      if (flag.vflag == 1) {
        if ((ch >= 0 && ch <= 31) && ch != '\t' && ch != '\n') {
          printf("^");
          ch = ch + 64;

        } else if (ch == 127) {
          printf("^");
          ch = ch - 64;
        }
      }
      putchar(ch);
    }
    fclose(filename);
  } else {
    fprintf(stderr, "No such file or directory: %s\n", path);
  }
}
