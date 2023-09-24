
all: test_ok

test: test.c cson.h
	gcc -ggdb -Wall -Wpedantic -Werror test.c -o test

test_ok: test
	./test
