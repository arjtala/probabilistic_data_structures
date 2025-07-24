#ifndef BITARRAY_H
#define BITARRAY_H

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define BITS_IN_TYPE(type) (CHAR_BIT * sizeof(type))

typedef struct {
	uint64_t *data;
	size_t size;        // Number of elements
} BitArray;

BitArray *createBitArray(size_t num_bits) {
	BitArray *bitv = malloc(sizeof(BitArray));
	if (bitv == NULL) {
		perror("Failed to allocate BitArray struct");
		return NULL;
	}

	size_t bits_per_unit = BITS_IN_TYPE(uint64_t);
	size_t num_units = (num_bits + bits_per_unit - 1) / bits_per_unit;
	bitv->data = calloc(num_units, sizeof(uint64_t));
    if (NULL==bitv->data) {
        fprintf(stderr, "Failed to allocate BitArray data.\n");
		free(bitv);
        exit(EXIT_FAILURE);
    }
    bitv->size = num_bits;
    return bitv;
}

void freeBitArray(BitArray *bitv) {
    free(bitv->data);
    free(bitv);
}

void setBit(BitArray *bitv, size_t index) {
	if (index >= bitv->size) {
        fprintf(stderr, "Out of bounds bit_idx=%zu, vect->size=%zu\n",
                            index, bitv->size);
        exit(EXIT_FAILURE);
	}
    size_t chunk_offset = index /  BITS_IN_TYPE(uint64_t);
    size_t bit_offset = index & (BITS_IN_TYPE(uint64_t)-1);
    uint64_t *byte = &(bitv->data[chunk_offset]);
	*byte |= ((uint64_t)1) << bit_offset;
}

void clearBit(BitArray *bitv, size_t index) {
	if (index >= bitv->size) {
        fprintf(stderr, "Out of bounds bit_idx=%zu, vect->size=%zu\n",
                            index, bitv->size);
        exit(EXIT_FAILURE);
	}
    size_t chunk_offset = index /  BITS_IN_TYPE(uint64_t);
    size_t bit_offset = index & (BITS_IN_TYPE(uint64_t)-1);
    uint64_t *byte = &(bitv->data[chunk_offset]);
	*byte &= ~(1 << bit_offset);
}


void flipBit(BitArray *bitv, size_t index) {
	if (!bitv || index >= bitv->size) {
        fprintf(stderr, "Out of bounds bit_idx=%zu, vect->size=%zu\n",
                            index, bitv->size);
        exit(EXIT_FAILURE);
	}
    size_t chunk_offset = index /  BITS_IN_TYPE(uint64_t);
    size_t bit_offset = index & (BITS_IN_TYPE(uint64_t)-1);
    uint64_t *byte = &(bitv->data[chunk_offset]);
	*byte ^= ((uint64_t)1) << bit_offset;
}

int getBit(BitArray *bitv, size_t index) {
	if (index >= bitv->size) {
        fprintf(stderr, "Out of bounds bit_idx=%zu, vect->size=%zu\n",
                            index, bitv->size);
		return -1;
	}
    size_t chunk_offset = index / BITS_IN_TYPE(uint64_t);
    size_t bit_offset = index & (BITS_IN_TYPE(uint64_t)-1);
    uint64_t byte = bitv->data[chunk_offset];
    return (byte>>bit_offset) & 1;
}

void printBits(BitArray *bitv, size_t size) {
	if (bitv->data == NULL) {
		return;
	}
    printf("[ ");
    for (size_t i = 0; i < size; i++) {
        printf("%d ", getBit(bitv, i) ? 1 : 0);
    }
    printf("]\n");
}

void uint64_to_binary(uint64_t input, BitArray *bitv) {
	printf("%lu\n", (uint64_t)8*sizeof(uint64_t));
    if (bitv->size < 8*sizeof(uint64_t)) {
        fprintf(stderr, "BitArray size too small for %lu\n", input);
        exit(EXIT_FAILURE);
    }
    for (int i = 63; i >= 0; --i) {
		unsigned int bit = input >> i & 1;
		if (bit) {
			setBit(bitv, 63-i);
			printBits(bitv, 8*sizeof(uint64_t));
			for (int c = 63; c > i; c--) {
				printf("_");
			}
			printf("^\n");
		}
    }
}

void printBinary(uint64_t num, size_t len) {
    for (int i = len-1; i >= 0; i--) {
		uint64_t mask = (uint64_t)1 << i;
		if (num & mask) {
			printf("1");
		} else {
			printf("0");
		}
    }
    printf("\n");
}

size_t msb_position(uint64_t w, size_t max) {
	if (w == 0) return max + 1;
    return __builtin_clzll(w) + 1; // count leading zeros
}

#endif
