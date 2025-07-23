#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "bloom.h"
#include "hll.h"
#include "utilities.h"


void test_batch_phrases(int p, const char *filename) {

	const size_t size = (1ULL << p)*sizeof(uint64_t);
	BloomFilter *filter = BloomFilter_default(size);
	HLL *hll = HLL_default(p);

    int count;
	char **sentences = load_sentences(filename, &count);
	if (!sentences) {
		fprintf(stderr, "Failed to load sentences from file\n");
		exit(EXIT_FAILURE);
	}
	printf("Loaded %d sentences\n", count);

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
	if (!filter->bitv) {
		fprintf(stderr, "Error: filter->bitv is NULL\n");
		exit(EXIT_FAILURE);
	}
	if (!filter->bitv->data) {
		fprintf(stderr, "Error: filter->bitv->data is NULL\n");
		exit(EXIT_FAILURE);
	}
	if (filter->bitv->size == 0) {
		fprintf(stderr, "Error: filter->bitv->size is zero\n");
		exit(EXIT_FAILURE);
	}
	double utilization = 100.0 * (double)countBitsSet(filter->bitv) / (double)filter->bitv->size;
	printf("Bloom filter Bitvector utilization: %.2f%%\n", utilization);

	printf("Inserted %d sentences (%zu unique)\n", count, unique);
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

int main(int argc, char *argv[]) {
    // Check if we have enough arguments
    if (argc < 2) {
        printf("Usage: %s <p_value>\n", argv[0]);
        return 1;
    }

	(void)argc;
	int p = atoi(argv[1]);
	printf("Creating HLL with parameter `p`=%d\n", p);

	const char *data = argv[2];
	const char *filename = argv[3];

	printSeparator();
	test_HLL(p, data);

	printSeparator();
	test_merge_two(p);

	printSeparator();
	test_batch_phrases(p, filename);

	return 0;
}
