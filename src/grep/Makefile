FLAGS = -Wall -Werror -Wextra -std=c11 -pedantic

all: s21_grep

rebuild: clean all

s21_grep: s21_grep.c
	gcc $(FLAGS) s21_grep.c -o s21_grep

test: s21_grep
	bash test_func_grep.sh
	
clean:
	rm -rf s21_grep
 