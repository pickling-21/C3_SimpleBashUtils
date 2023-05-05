#include "s21_grep.h"

int main(int argc, char *argv[]) {
  char patterns[SIZE][SIZE] = {0};
  int opt_ind = 0;
  flags option = {0};
  grep_values value = {0};

  int file_location = 0;
  opt_ind = parser(argc, argv, &option, &value, patterns);

  if (argc >= 3 && option.errorflag != 1 && opt_ind != argc) {
    file_location = find_pattern(opt_ind, argv, patterns);
    while (file_location < argc) {
      if (argv[file_location + 1] != NULL) {
        value.count_files = value.count_files + 1;
      }
      value.path = argv[file_location];
      grep(value, option, patterns);
      file_location++;
    }
  }
}

int find_pattern(int opt_ind, char *argv[], char patterns[SIZE][SIZE]) {
  int file_location = 0;
  if (*patterns[0] == 0) {
    file_location = opt_ind + 1;
    strcpy(patterns[0], argv[optind]);
  } else {
    file_location = opt_ind;
  }
  return file_location;
}

int parser(int argc, char *argv[], flags *flag, grep_values *value,
           char patterns[SIZE][SIZE]) {
  int opt = 0;
  opterr = 0;

  while ((opt = getopt_long(argc, argv, "e:ivclnhsf:o", NULL, NULL)) != -1) {
    switch (opt) {
      case 'e':
        while (*patterns[value->count_pattern] != 0) {
          value->count_pattern = value->count_pattern + 1;
        }
        strcpy(patterns[value->count_pattern], optarg);
        value->count_pattern = value->count_pattern + 1;
        flag->eflag = 1;
        break;
      case 'i':
        flag->iflag = 1;
        break;
      case 'v':
        flag->vflag = 1;
        break;
      case 'c':
        flag->cflag = 1;
        break;
      case 'l':
        flag->lflag = 1;
        break;
      case 'n':
        flag->nflag = 1;
        break;
      case 'h':
        flag->hflag = 1;
        break;
      case 's':
        flag->sflag = 1;
        break;
      case 'f':
        while (*patterns[value->count_pattern] != 0) {
          value->count_pattern = value->count_pattern + 1;
        }
        f_flag(optarg, patterns, value);
        flag->fflag = 1;
        break;
      case 'o':
        flag->oflag = 1;
        break;
      case '?':
        flag->errorflag = 1;
        break;
    }
  }
  return optind;
}

void grep(grep_values value, flags flag, char pattern[SIZE][SIZE]) {
  FILE *filename;
  char string[SIZE] = " ";
  int count_lines = 0;
  int count_matched_lines = 0;

  int comp_flag1 = REG_EXTENDED;

  int stop = 0;

  int result_regex = 243;
  regex_t preg;

  if (flag.vflag == 1 || flag.cflag == 1 || flag.lflag == 1) flag.oflag = 0;

  if (access(value.path, F_OK) == 0) {
    filename = fopen(value.path, "r");

    if (value.count_pattern == 0 && flag.eflag == 0 && flag.fflag == 0)
      value.count_pattern = 1;
    if (flag.iflag == 1) comp_flag1 = REG_ICASE;

    while ((fgets(string, SIZE, filename)) && (stop == 0)) {
      int findline = 0;
      count_lines++;
      int pattern_no_match = 0;

      if (strchr(string, '\n') == NULL) strcat(string, "\n");

      for (int i = 0; i < value.count_pattern; i++) {
        int match = 0;

        regcomp(&preg, pattern[i], comp_flag1 | REG_NEWLINE);
        result_regex = regexec(&preg, string, 0, NULL, 0);

        if (result_regex == 0 && flag.vflag == 0) match = 1;

        if (result_regex == REG_NOMATCH && flag.vflag == 1) {
          pattern_no_match++;
          if (pattern_no_match == value.count_pattern) match = 1;
        }
        if (flag.lflag == 1 && match == 1 && flag.cflag == 0) {
          printf("%s\n", value.path);
          match = 0;
          stop = 1;
        }
        if (flag.cflag == 1 && match == 1) {
          count_matched_lines++;
          match = 0;
        }
        if (findline == 0 && match == 1) {
          printing_strings(value, flag, count_lines, string);
          findline++;
        }
        if (flag.oflag == 1) {
          flag_o(string, preg);
        }
        regfree(&preg);
      }
    }
    if (flag.cflag == 1) {
      flag_c_l(value, flag, count_matched_lines);
    }
    fclose(filename);
  } else if (flag.sflag == 0) {
    fprintf(stderr, "No such file or directory: %s\n", value.path);
  }
}

void f_flag(char *path, char pattern[SIZE][SIZE], grep_values *value) {
  FILE *filename;
  int lenght = 0;
  if (access(path, F_OK) == 0) {
    filename = fopen(path, "r");
    while (!feof(filename)) {
      fgets(pattern[value->count_pattern], SIZE, filename);
      lenght = strlen(pattern[value->count_pattern]);

      if (pattern[value->count_pattern][0] != '\n' &&
          pattern[value->count_pattern][lenght - 1] == '\n') {
        pattern[value->count_pattern][lenght - 1] = '\0';
      }
      value->count_pattern = value->count_pattern + 1;
    }
    fclose(filename);
  } else {
    printf("error file");
  }
}

void flag_c_l(grep_values value, flags flag, int count_matched_lines) {
  if (value.count_files >= 1 && flag.hflag == 0) {
    printf("%s:", value.path);
  }
  if (flag.lflag == 0) {
    printf("%d\n", count_matched_lines);
  } else if (flag.lflag == 1) {
    if (count_matched_lines > 0) {
      printf("1\n");
      printf("%s\n", value.path);
    } else {
      printf("0\n");
    }
  }
}

void printing_strings(grep_values value, flags flag, int count_lines,
                      char *string) {
  if (value.count_files >= 1 && flag.hflag == 0) printf("%s:", value.path);
  if (flag.nflag == 1) printf("%d:", count_lines);
  if (flag.oflag == 0) printf("%s", string);
}

void flag_o(char *string, regex_t preg) {
  regmatch_t pmatch[1];
  while (regexec(&preg, string, 1, pmatch, 0) == 0) {
    for (int j = 0; j < pmatch->rm_eo; j++) {
      if (j >= pmatch->rm_so) {
        printf("%c", string[j]);
      }
      string[j] = 127;
    }
    printf("\n");
  }
}
