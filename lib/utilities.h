#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 2048
#define MAX_LINES 1000000
#define SEPARATOR "\n\n******************************\n"
#define ASSERT(condition, expected, value) do {	\
	printf("%d == %d\n", expected, value);			\
    if (!(condition)) { \
        fprintf(stderr, "!!! Assertion failed: expected %d but got %d\n", expected, value); \
		exit(EXIT_FAILURE); \
    } \
} while(0)

void printSeparator(void) {
	printf("%s", SEPARATOR);
}

char **load_sentences(const char *filename, int *out_count) {
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
    int count = 0;

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
            fprintf(stderr, "Too many lines (max %d)\n", MAX_LINES);
            break;
        }
    }

    fclose(file);
	*out_count = count;
	return sentences;
}
