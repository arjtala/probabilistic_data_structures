#ifndef BLOOM_H
#define BLOOM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "../lib/hash.h"
#include "../lib/bitarray.h"

typedef struct {
	BitArray *bits;
	hash64_func *hash_functions;
	size_t num_functions;
	size_t num_items;
} BloomFilter;

BloomFilter *BloomFilter_new(size_t size, size_t num_functions, ...);
BloomFilter *BloomFilter_default(size_t size);
void BloomFilter_put(BloomFilter *filter, const void *data, size_t size);
void BloomFilter_putStr(BloomFilter *filter, const char *str);
bool BloomFilter_exists(BloomFilter *filter, const void *data, size_t size);
bool BloomFilter_strExists(BloomFilter *filter, const char *str);
void free_BloomFilter(BloomFilter *filter);
size_t countBitsSet(BitArray *bits);

#endif
