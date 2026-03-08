#include "utilities.h"
#include <stdio.h>
#include <cstdio>
#include <cstdlib>

void printSeparator(void) {
  printf("%s", SEPARATOR);
}

char **load_sentences(const char *filename, long *out_count) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    printf("%s", filename);
    perror("Failed to open file");
    exit(EXIT_FAILURE);
  }

  size_t capacity = INITIAL_SENTENCES_MAX_LINES;
  char **sentences = malloc(capacity * sizeof(char *));
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

    if ((size_t)count >= capacity) {
      capacity *= 2;
      char **tmp = realloc(sentences, capacity * sizeof(char *));
      if (NULL == tmp) {
        fprintf(stderr, "Memory re-allocation failure...\n");
        exit(EXIT_FAILURE);
      }
      sentences = tmp;
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
