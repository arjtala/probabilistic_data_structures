#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 2048
#define INITIAL_CAPACITY 1024
#define SEPARATOR "\n\n******************************\n"
#define ASSERT(condition, expected, value) do {	\
	printf("%d == %d\n", expected, value);			\
    if (!(condition)) { \
        fprintf(stderr, "!!! Assertion failed: expected %d but got %d\n", expected, value); \
		exit(EXIT_FAILURE); \
    } \
} while(0)
#define RUN_TEST(fn, ...) do {    \
    printSeparator();             \
    printf("[%s]\n", #fn);        \
    fn(__VA_ARGS__);              \
} while (0)

void printSeparator(void);
char **load_sentences(const char *filename, long *out_count);
void format_with_commas(unsigned long long n, char *out);
