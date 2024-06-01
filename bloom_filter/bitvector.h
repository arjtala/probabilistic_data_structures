#ifndef BITVECTOR_H
#define BITVECTOR_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/*
To implement a Bloom filter we need an auxilary data
structure which helps us manipulate bits. This data structure
compactly stores bits.

https://en.wikipedia.org/wiki/Bit_array
**/

#define BITS_PER_BYTE (sizeof(unsigned int) * 8)
#define BITS_IN_TYPE(type) (BITS_PER_BYTE * (sizeof(type)))

typedef struct {
	unsigned int *data;
	size_t size;        // Number of elements
} BitVector;

BitVector *createBitVector(size_t num_bits) {
	BitVector *bitv = malloc(sizeof(BitVector));
	if (bitv == NULL) {
		return NULL;
	}

	size_t mem_size = num_bits / BITS_IN_TYPE(uint32_t);
    if (!(num_bits%BITS_IN_TYPE(u_int32_t))) {
        mem_size++;
    }
	bitv->data = calloc(mem_size, sizeof(*(bitv->data)));
    if (NULL==bitv->data) {
        fprintf(stderr, "Out of memory.\n");
        exit(EXIT_FAILURE);
    }
    bitv->size = num_bits;
    return bitv;
}

void freeBitVector(BitVector *bitv) {
    free(bitv->data);
    free(bitv);
}


void setBit(BitVector *bitv, size_t index) {
	if (index >= bitv->size) {
        fprintf(stderr, "Out of bounds bit_idx=%zu, vect->size=%zu\n",
                            index, bitv->size);
        exit(EXIT_FAILURE);
	}
    size_t chunk_offset = index /  BITS_IN_TYPE(uint32_t);
    size_t bit_offset = index & (BITS_IN_TYPE(uint32_t)-1);
    uint32_t *byte = &(bitv->data[chunk_offset]);
	*byte |= ((uint32_t)1) << bit_offset;
}

void clearBit(BitVector *bitv, size_t index) {
	if (index >= bitv->size) {
        fprintf(stderr, "Out of bounds bit_idx=%zu, vect->size=%zu\n",
                            index, bitv->size);
        exit(EXIT_FAILURE);
	}
    size_t chunk_offset = index /  BITS_IN_TYPE(uint32_t);
    size_t bit_offset = index & (BITS_IN_TYPE(uint32_t)-1);
    uint32_t *byte = &(bitv->data[chunk_offset]);
	*byte &= ~(1 << bit_offset);
}


void flipBit(BitVector *bitv, size_t index) {
	if (index >= bitv->size) {
        fprintf(stderr, "Out of bounds bit_idx=%zu, vect->size=%zu\n",
                            index, bitv->size);
        exit(EXIT_FAILURE);
	}
    size_t chunk_offset = index /  BITS_IN_TYPE(uint32_t);
    size_t bit_offset = index & (BITS_IN_TYPE(uint32_t)-1);
    uint32_t *byte = &(bitv->data[chunk_offset]);
	*byte &= *byte ^= ((uint32_t)1) << bit_offset;
}

int getBit(BitVector *bitv, size_t index) {
	if (index >= bitv->size) {
        fprintf(stderr, "Out of bounds bit_idx=%zu, vect->size=%zu\n",
                            index, bitv->size);
		return -1;
	}
    size_t chunk_offset = index / BITS_IN_TYPE(uint32_t);
    size_t bit_offset = index & (BITS_IN_TYPE(uint32_t)-1);
    uint32_t byte = bitv->data[chunk_offset];
    return (byte>>bit_offset) & 1;
}

void printBits(BitVector *bitv, size_t size) {
	if (bitv->data == NULL) {
		return;
	}
	printf("[");
    for (size_t i = 0; i < size; i++) {
        printf("%d ", getBit(bitv, i));
    }
	printf("]\n");
}

#endif
