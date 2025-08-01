#define _POSIX_C_SOURCE 200809L
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include "bloom.h"
#include "hll.h"
#include "utilities.h"


void test_time_insertion(int p, char *filename) {
	HLL *hll = HLL_default(p);

    long count;
	char count_formatted[32];

	char **sentences = load_sentences(filename, &count);
	format_with_commas(count, count_formatted);

	if (!sentences) {
		fprintf(stderr, "Failed to load sentences from file\n");
		exit(EXIT_FAILURE);
	}
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
	for (int i = 0; i < count; ++i) {
		HLL_add(hll, sentences[i], strlen(sentences[i]));
		free(sentences[i]);
	}
	clock_gettime(CLOCK_MONOTONIC, &end);
	double elapsed_sec = (end.tv_sec - start.tv_sec) +
		(end.tv_nsec - start.tv_nsec) / 1e9;
	double num_elements = HLL_count(hll);
	double memory_footprint = HLL_memory_usage(hll) / 1024.0 / 1024.0;
	printf("Processed %s records in %.4f seconds, memory ~= %0.4f, cardinality ~= %f\n",
		   count_formatted, elapsed_sec, memory_footprint, num_elements);
}

void test_batch_phrases(int p, const char *filename) {

	const size_t size = (1ULL << p)*sizeof(uint64_t);
	BloomFilter *filter = BloomFilter_default(size);
	HLL *hll = HLL_default(p);

    long count;
	char **sentences = load_sentences(filename, &count);
	if (!sentences) {
		fprintf(stderr, "Failed to load sentences from file\n");
		exit(EXIT_FAILURE);
	}
	printf("Loaded %ld sentences\n", count);

	size_t unique = 0;
	for (int i = 0; i < count; ++i) {

		char *s = sentences[i];
		if (!s) {
			fprintf(stderr, "NULL sentence at index %d\n", i);
			continue;
		}

		size_t len = strlen(s);
		HLL_add(hll, s, len);

		if (!BloomFilter_strExists(filter, s)) {
			unique++;
		}
		BloomFilter_putStr(filter, s);

		free(s);  // Only if strdup/malloc used
	}

	if (!filter) {
		fprintf(stderr, "Error: filter is NULL\n");
		exit(EXIT_FAILURE);
	}
	if (!filter->bits) {
		fprintf(stderr, "Error: filter->bits is NULL\n");
		exit(EXIT_FAILURE);
	}
	if (!filter->bits->data) {
		fprintf(stderr, "Error: filter->bits->data is NULL\n");
		exit(EXIT_FAILURE);
	}
	if (filter->bits->size == 0) {
		fprintf(stderr, "Error: filter->bits->size is zero\n");
		exit(EXIT_FAILURE);
	}
	double utilization = 100.0 * (double)countBitsSet(filter->bits) / (double)filter->bits->size;
	printf("Bloom filter BitArray utilization: %.2f%%\n", utilization);

	printf("Inserted %ld sentences (%zu unique)\n", count, unique);
	double num_elements = HLL_count(hll);
	printf("Number of elements ~= %f\n", num_elements);
	freeHLL(hll);
	free(sentences);
	free_BloomFilter(filter);
}

void test_HLL(int p, const char *data) {
	HLL *hll = HLL_default(p);
	double num_elements = 0;
	num_elements = HLL_count(hll);
	printf("Number of elements ~= %f\n", num_elements);

	printf("Inserting `%s` into HLL\n", data);
	int len = strlen(data);
	HLL_add(hll, data, len);
	num_elements = HLL_count(hll);
	printf("Number of elements ~= %f\n", num_elements);

	num_elements = 20.0;
	printf("Inserting %f more elements...\n", num_elements);
	for (int i = 0; i < num_elements; ++i) {
		char buf[32];
		snprintf(buf, sizeof(buf), "elem_%d", i);
		HLL_add(hll, buf, strlen(buf));
	}

	int non_zero = 0;
	for (size_t i = 0; i < hll->m; i++) {
		if (hll->registers[i]) non_zero++;
	}
	printf("Non-zero registers: %d\n", non_zero);
	num_elements = HLL_count(hll);
	printf("Number of elements ~= %f\n", num_elements);
}

void test_merge_two(int p) {

	HLL *hll1 = HLL_default(p);
	const char *pb = "peanut butter";
	printf("Inserting `%s` into HLL 1\n", pb);
	HLL_add(hll1, pb, strlen(pb));

	HLL *hll2 = HLL_default(p);
	const char *banana = "banana";
	printf("Inserting `%s` into HLL 2\n", banana);
	HLL_add(hll2, banana, strlen(banana));

	HLL *merged = HLL_merge_copy(hll1, hll2);
	if (merged) {
		printf("Merged count: ~%.2f\n", HLL_count(merged));
		freeHLL(merged);
	}
	freeHLL(hll1);
	freeHLL(hll2);
}

void test_hll_accuracy(int p) {
    HLL *hll = HLL_default(p);
    int true_count = 100000;

	char buffer[64];
	for (int i = 1; i < true_count; ++i) {
		snprintf(buffer, sizeof(buffer), "item_%d", i);
		HLL_add(hll, buffer, strlen(buffer));
	}

	double estimate = HLL_count(hll);
	printf("True count: %d, HLL estimate: %.2f\n", true_count, estimate);
	printf("Relative error %.4f%%\n", 100 * fabs(estimate - true_count) / true_count);
	freeHLL(hll);
}

void test_hll_duplicates(int p) {
	HLL *hll = HLL_default(p);
	const char *value = "constant string";
	size_t len = strlen(value);

	for (int i = 0; i < 100000; ++i) {
		HLL_add(hll, value, len);
	}

	double estimate = HLL_count(hll);
	printf("Estimate with 100,000 duplicates of same string: %.2f\n", estimate);
	freeHLL(hll);
}

int main(int argc, char *argv[]) {
    // Check if we have enough arguments
    if (argc < 2) {
        printf("Usage: %s <p_value>\n", argv[0]);
        return 1;
    }

	int p = atoi(argv[1]);
	const char *data = argv[2];
	const char *filename = argv[3];

    char prefix[64];
    char suffix[64];
    char dynamic_filename[128];

    // Extract the prefix and suffix from base_filename
    // Assumes base_filename format like "phrases.txt"
    const char *dot = strrchr(filename, '.');
    if (dot == NULL) {
        fprintf(stderr, "Invalid filename: %s\n", filename);
        return 1;
    }

    // Copy prefix (e.g., "phrases")
    size_t prefix_len = dot - filename;
    strncpy(prefix, filename, prefix_len);
    prefix[prefix_len] = '\0';

    // Copy suffix (e.g., ".txt")
    strcpy(suffix, dot);

	RUN_TEST(test_HLL, p, data);
	RUN_TEST(test_merge_two, p);
	RUN_TEST(test_hll_accuracy, p);
	RUN_TEST(test_hll_duplicates, p);
	RUN_TEST(test_batch_phrases, p, filename);

	char f[64];
    for (int i = 5; i <= 9; ++i) {
        snprintf(dynamic_filename, sizeof(dynamic_filename), "%s_%d_half%s", prefix, i, suffix);
        snprintf(f, sizeof(f), dynamic_filename, i);
        RUN_TEST(test_time_insertion, p, f);
    }

	return 0;
}
