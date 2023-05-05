FLAGS = -Wall -Werror -Wextra -std=c11

all: s21_cat

rebuild: clean all

s21_cat: s21_cat.c
	gcc $(FLAGS) s21_cat.c -o s21_cat
test: s21_cat
	bash test_func_cat.sh
clean:
	rm -rf s21_cat