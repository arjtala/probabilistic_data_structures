#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include "bitarray.h"
#include "bloom.h"
#include "hash.h"
#include "utilities.h"


void printArray(int *arr) {
	printf("[");
	for (size_t i = 0; i < sizeof(arr); i++) {
		printf("%d ", arr[i]);
	}
	printf("]\n");
}

char * createExpected(size_t size) {
	char open = '[';
	char close = ']';
	char *str = malloc(2*size + 4);
	str[0] = open;
	str[1] = ' ';

	str[2*size + 2] = close;
	str[2*size + 3] = '\0';

	for (size_t i = 0; i < size; i++) {
		str[2*i+2] = '0';
		str[2*i+3] = ' ';
	}
	return str;
}

void flipExpectedValue(char* expected, size_t index, size_t total) {
	if (index >= total) {
		printf("Error: Index out of bounds.\n");
		return;
	}
	size_t curr_index = 2*index + 2;
	char curr = expected[curr_index];
    if (curr == '0') {
            expected[curr_index] = '1';
    } else if (curr == '1') {
        expected[curr_index] = '0';
	} else {
		printf("Unexpected value in simulated bit-array: %c\n", curr);
	}
}

void test_BitArray_display(void) {
	size_t sizeOfExpected = 6;
    BitArray *bv = createBitArray(sizeOfExpected);
	char* expected = createExpected(sizeOfExpected);
	printf("Built expected BitArray (allocated size = %lu): %s\n", sizeof(expected), expected);

	printf("Set position 3:\n");
    setBit(bv, 3); // Set bit at index 3
	flipExpectedValue(expected, 3, sizeOfExpected);
	printf("Expected: ");
	printf("%s\n", expected);
	printf("Result:   ");
	printBits(bv, sizeOfExpected);

	printf("Flip position 5:\n");
    setBit(bv, 5); // Toggle bit at index 5
	flipExpectedValue(expected, 5, sizeOfExpected);
	printf("Expected: ");
	printf("%s\n", expected);
	printf("Result:   ");
	printBits(bv, sizeOfExpected);

	printf("Clear position 3:\n");
    flipBit(bv, 3); // Flip bit at index 3
	flipExpectedValue(expected, 3, sizeOfExpected);
	printf("Expected: ");
	printf("%s\n", expected);
	printf("Result:   ");
	printBits(bv, sizeOfExpected);
}

void test_BitArray_values(void) {
	int val, exp;

	size_t sizeOfExpected = 6;
	size_t pos = 5;
    BitArray *bv = createBitArray(sizeOfExpected);
    setBit(bv, pos);
	val = getBit(bv, pos);
	exp = 1;
	printf("Setting bit in position %zu: ", pos);
	ASSERT(val == exp, exp, val);

	clearBit(bv, pos);
	val = getBit(bv, pos);
	exp = 0;
	printf("Clearing bit in position %zu: ", pos);
	ASSERT(val == exp, exp, val);
}

void test_bloom_filter(void) {
	bool exp, val;
    BloomFilter *filter = BloomFilter_default(64*8);

	uint64_t x = 9;
	BloomFilter_put(filter, &x, sizeof(x));
	val = BloomFilter_exists(filter, &x, sizeof(x));
	exp = true;
	printf("Inserting value %llu into filter: ", x);
	ASSERT(exp == val, exp, val);

	exp = false;
	val = BloomFilter_strExists(filter, "abc");
	printf("Checking existence of value `abc` in filter: ");
	ASSERT(exp == val, exp, val);

	exp = true;
    BloomFilter_putStr(filter, "abc");
	val = BloomFilter_strExists(filter, "abc");
	printf("Inserting value `abc` into filter: ");
	ASSERT(exp == val, exp, val);

	char data[] = "abc";
	size_t data_len = strlen(data);
	size_t N = 8;
	uint64_t hash = sdbm(data, data_len);
	printf("Hash: %llu (%llu)", hash, hash % N);

	BitArray *tmp_bit = createBitArray(N);
	setBit(tmp_bit, hash % N);
	printBits(tmp_bit, N);

	const int num_elements = 1000;
	printf("Inserting %d more elements...\n", num_elements);
	for (int i = 0; i < num_elements; ++i) {
		char buf[32];
		snprintf(buf, sizeof(buf), "elem_%d", i);
		BloomFilter_putStr(filter, buf);
	}

	printf("BitArray utilization: %.2f%%\n",
		   100.0 * countBitsSet(filter->bitv) / filter->bitv->size);
}

int main(void) {
	RUN_TEST(test_BitArray_display);
	RUN_TEST(test_BitArray_values);
	RUN_TEST(test_bloom_filter);
    return 0;
}
