#include <stdio.h>
#include <string.h>
#include "bitvector.h"
#include "hash.h"
#include "bloom.h"

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
	char *str = malloc(2*size + 2);
	str[0] = open;
	str[2*size + 1] = close;
	for (size_t i = 0; i < size; i++) {
		str[2*i+1] = '0';
		str[2*i+2] = ' ';
	}
	return str;
}

void flipExpectedValue(char* expected, size_t index, size_t total) {
	if (index >= total) {
		printf("Error: Index out of bounds.\n");
		return;
	}
	size_t curr_index = 2*index + 1;
	char curr = expected[curr_index];
    if (curr == '0') {
            expected[curr_index] = '1';
    } else if (curr == '1') {
        expected[curr_index] = '0';
	} else {
		printf("Unexpected value in simulated bit-array: %c", curr);
	}
}

void test_bitvector_display(void) {
	size_t sizeOfExpected = 6;
    BitVector *bv = createBitVector(sizeOfExpected);
	char* expected = createExpected(sizeOfExpected);
	printf("Built expected bitvector (allocated size = %lu): %s\n", sizeof(expected), expected);

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
    flipBit(bv, 3); // Clear bit at index 7
	flipExpectedValue(expected, 3, sizeOfExpected);
	printf("Expected: ");
	printf("%s\n", expected);
	printf("Result:   ");
	printBits(bv, sizeOfExpected);
}

void test_bitvector_values(void) {
	int val, exp;

	size_t sizeOfExpected = 6;
    BitVector *bv = createBitVector(sizeOfExpected);
    setBit(bv, 5);
	val = getBit(bv, 5);
	exp = 1;
	ASSERT(val == exp, exp, val);

	clearBit(bv, 5);
	val = getBit(bv, 5);
	exp = 0;
	ASSERT(val == exp, exp, val);
}

void test_bloom_filter(void) {
	bool exp, val;
    BloomFilter *filter = BloomFilter_default(1024);

	uint32_t x = 9;
	BloomFilter_put(filter, &x, sizeof(x));
	val = BloomFilter_exists(filter, &x, sizeof(x));
	exp = true;
	ASSERT(exp == val, exp, val);

	exp = false;
	val = BloomFilter_strExists(filter, "abc");
	ASSERT(exp == val, exp, val);

	exp = true;
    BloomFilter_putStr(filter, "abc");
	val = BloomFilter_strExists(filter, "abc");
	ASSERT(exp == val, exp, val);

	char data[] = "abc";
	size_t data_len = strlen(data);
	size_t N = 8;
	uint32_t hash = sdbm(data, data_len);
	printf("Hash: %d (%zu)", hash, hash % N);
	/* printf("["); */
	/* for (size_t i = 0; i < sizeof(hash); i++) { */
	/* 	printf("%d ", hash[i]); */
	/* } */
	/* printf("]\n"); */

	BitVector *tmp_bit = createBitVector(N);
	setBit(tmp_bit, hash % N);
	printBits(tmp_bit, N);
}

int main(void) {

	printSeparator();
	test_bitvector_display();

	printSeparator();
	test_bitvector_values();

	printSeparator();
	test_bloom_filter();

	printf("All tests passed successfully!\n");
    return 0;
}
