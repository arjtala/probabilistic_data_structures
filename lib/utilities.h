#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 2048
#define MAX_LINES 100000000000
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

void printSeparator(void) {
	printf("%s", SEPARATOR);
}

char **load_sentences(const char *filename, long *out_count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
		exit(EXIT_FAILURE);
    }

    char **sentences = malloc(sizeof(char *) * MAX_LINES);
    if (!sentences) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    char buffer[MAX_LINE_LEN];
    long count = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        // Remove newline character if present
        buffer[strcspn(buffer, "\n")] = '\0';

		if (strlen(buffer) == 0) {
			continue;
		}

        // Allocate memory and copy the sentence
        sentences[count] = strdup(buffer);
        if (!sentences[count]) {
            fprintf(stderr, "Memory allocation failed\n");
			exit(EXIT_FAILURE);
        }
        count++;
        if (count >= MAX_LINES) {
            fprintf(stderr, "Too many lines (max %ld)\n", MAX_LINES);
            break;
        }
    }

    fclose(file);
	*out_count = count;
	return sentences;
}

// Output buffer must be large enough (e.g., 32+ chars)
void format_with_commas(unsigned long long n, char *out) {
    char buffer[32];
    sprintf(buffer, "%llu", n);

    int len = strlen(buffer);
    int comma_count = (len - 1) / 3;
    int out_len = len + comma_count;

    out[out_len] = '\0';

    int i = len - 1;
    int j = out_len - 1;
    int digit_count = 0;

    while (i >= 0) {
        out[j--] = buffer[i--];
        digit_count++;
        if (digit_count == 3 && i >= 0) {
            out[j--] = ',';
            digit_count = 0;
        }
    }
}
