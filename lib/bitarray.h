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

BitArray *createBitArray(size_t num_bits);
void freeBitArray(BitArray *bits);
void printBits(BitArray *bits, size_t size);
void unit_to_binary(unit_t input, BitArray *bits);
void printBinary(unit_t num, size_t len);
size_t msb_position(unit_t w, size_t max);

#endif
