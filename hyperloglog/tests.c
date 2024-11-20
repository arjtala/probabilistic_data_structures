#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "hll.h"

uint64_t isolateQBits(uint64_t num, int p, int q) {
    // Shift right by p to align q bits at the rightmost position
    uint64_t q_bits = num >> p;
    // Mask to retain only the q bits
    q_bits = q_bits & ((1ULL << q) - 1);
    return q_bits;
}
// Function to isolate the p bits
uint64_t isolatePBits(uint64_t num, int p, int q) {
    // Shift right by q to align p bits at the rightmost position
    uint64_t p_bits = num >> q;
    // Mask to retain only the p bits
    p_bits = p_bits & ((1ULL << p) - 1);
    return p_bits;
}

int findMSBIndexFromLeft(uint64_t q_bits) {
    if (q_bits == 0) return 0; // No bits are set
    int msb_index = 0;
    while (q_bits != 0) {
        q_bits >>= 1;
        msb_index++;
    }
    return msb_index; // Index of the MSB from the left
}

int main(int argc, char *argv[]) {

	int p = atoi(argv[1]);
	const char *data = argv[2];

	size_t offset = 64;
	int q = offset - p;
    uint64_t hash_value = hash_64(data, strlen(data));
    printf("Hash value: 0x%llX\n", hash_value);
	printf("%llu\n", hash_value);


	BitVector *bitv = createBitVector(offset);
	uint64_to_binary(hash_value, bitv);
	printBits(bitv, offset);

	uint64_t i;
	i = 1UL<<(sizeof(i)*8-1);
	size_t c = 0;
	while (i>0) {
		printf("hash_value & i = %d\n", (hash_value & i)>0);
		if (hash_value & i) {
			setBit(bitv, c);
		}
		c++;
		i >>= 1;
	}
	printBinary(hash_value, offset);
	printBits(bitv, offset);
	return 0;

	uint64_t a = isolatePBits(hash_value, p, q);
	uint64_t b = isolateQBits(hash_value, p, q);
	printf("a: %llu\n", a);
	printBinary(a, offset);
	printf("b: %llu\n", b);
	printBinary(b, offset);
	int k = findMSBIndexFromLeft(b);
	printf("%llu [%d]:\n", hash_value, k);
	printBinary(hash_value, offset);
	for (int _c = 0; _c < k; _c++) {
		if (_c < p) {
			printf("*");
		} else if (_c == p) {
			printf("|");
		} else {
			printf("_");
		}
	}
	printf("^\n");

	/* printf("Creating HLL with parameter `p`=%d\n", p); */
	/* HLL *hll = HLL_default(p); */
	/* printf("Inserting `%s` into HLL\n", data); */
	/* HLL_add(hll, data, strlen(data)) */;

	return 0;
}
