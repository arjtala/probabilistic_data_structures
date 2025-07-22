#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "hll.h"


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
	int len = strlen(data);

	HLL *hll = HLL_default(p);
	double count = 0;
	count = HLL_count(hll);
	printf("Number of elements ~= %f\n", count);

	printf("Inserting `%s` into HLL\n", data);
	HLL_add(hll, data, len);
	count = HLL_count(hll);
	printf("Number of elements ~= %f\n", count);

	const int num_elems = 20;
	printf("Inserting %d more elements...\n", num_elems);
	for (int i = 0; i < num_elems; ++i) {
		char buf[32];
		snprintf(buf, sizeof(buf), "elem_%d", i);
		HLL_add(hll, buf, strlen(buf));
	}

	int non_zero = 0;
	for (size_t i = 0; i < hll->m; i++) {
		if (hll->registers[i]) non_zero++;
	}
	printf("Non-zero registers: %d\n", non_zero);
	count = HLL_count(hll);
	printf("Number of elements ~= %f\n", count);

	HLL *hll2 = HLL_default(p);
	const char *banana = "banana";
	HLL_add(hll2, banana, strlen(banana));

	HLL *merged = HLL_merge_copy(hll, hll2);
	if (merged) {
		printf("Merged count: ~%.2f\n", HLL_count(merged));
		freeHLL(merged);
	}
	freeHLL(hll);
	freeHLL(hll2);
	return 0;
}
