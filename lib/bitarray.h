#ifndef BITARRAY_H
#define BITARRAY_H

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef uint64_t unit_t;

#define BITS_IN_TYPE(type) (CHAR_BIT * sizeof(type))
#define BITS_PER_UNIT BITS_IN_TYPE(uint64_t)
#define BIT_INDEX(i) ((i) / BITS_PER_UNIT)
#define BIT_OFFSET(i) ((i) % BITS_PER_UNIT)
#define BIT_SET(bits, i) ((bits)[BIT_INDEX(i)] |= ((unit_t)1 << BIT_OFFSET(i)))
#define BIT_CLEAR(bits, i) ((bits)[BIT_INDEX(i)] &= ~((unit_t)1 << BIT_OFFSET(i)))
#define BIT_GET(bits, i) (((bits)[BIT_INDEX(i)] >> BIT_OFFSET(i)) & (unit_t)1)
#define BIT_FLIP(bits, i) ((bits)[BIT_INDEX(i)] ^= ((unit_t)1 << BIT_OFFSET(i)))

typedef struct {
	unit_t *data;
	size_t size;        // Number of elements
} BitArray;

BitArray *createBitArray(size_t num_bits) {
	BitArray *bits = malloc(sizeof(BitArray));
	if (bits == NULL) {
		perror("Failed to allocate BitArray struct");
		return NULL;
	}

	size_t num_units = (num_bits + BITS_PER_UNIT - 1) / BITS_PER_UNIT;
	bits->data = calloc(num_units, sizeof(unit_t));
    if (NULL==bits->data) {
        fprintf(stderr, "Failed to allocate BitArray data.\n");
		free(bits);
        exit(EXIT_FAILURE);
    }
    bits->size = num_bits;
    return bits;
}

void freeBitArray(BitArray *bits) {
    free(bits->data);
    free(bits);
}

void printBits(BitArray *bits, size_t size) {
	if (bits->data == NULL) {
		return;
	}
    printf("[ ");
    for (size_t i = 0; i < size; i++) {
        printf("%d ", BIT_GET(bits->data, i) ? 1 : 0);
    }
    printf("]\n");
}

void unit_to_binary(unit_t input, BitArray *bits) {
	printf("%llu\n", (unit_t)8*sizeof(unit_t));
    if (bits->size < 8*sizeof(unit_t)) {
        fprintf(stderr, "BitArray size too small for %llu\n", input);
        exit(EXIT_FAILURE);
    }
    for (int i = 63; i >= 0; --i) {
		unsigned int bit = input >> i & 1;
		if (bit) {
			BIT_SET(bits->data, 63-i);
			printBits(bits, 8*sizeof(unit_t));
			for (int c = 63; c > i; c--) {
				printf("_");
			}
			printf("^\n");
		}
    }
}

void printBinary(unit_t num, size_t len) {
    for (int i = len-1; i >= 0; i--) {
		unit_t mask = (unit_t)1 << i;
		if (num & mask) {
			printf("1");
		} else {
			printf("0");
		}
    }
    printf("\n");
}

size_t msb_position(unit_t w, size_t max) {
	if (w == 0) return max + 1;
    return __builtin_clzll(w) + 1; // count leading zeros
}

#endif
