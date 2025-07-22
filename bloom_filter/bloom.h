#ifndef BLOOM_H
#define BLOOM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "hash.h"
#include "bitvector.h"

typedef struct {
	BitVector *bitv;
	hash64_func *hash_functions;
	size_t num_functions;
	size_t num_items;
} BloomFilter;

BloomFilter *BloomFilter_new(size_t size, size_t num_functions, ...);
BloomFilter *BloomFilter_default(size_t size);
void BloomFilter_free(BloomFilter *filter);
void BloomFilter_put(BloomFilter *filter, const void *data, size_t size);
void BloomFilter_putStr(BloomFilter *filter, const char *str);
bool BloomFilter_exists(BloomFilter *filter, const void *data, size_t size);
bool BloomFilter_strExists(BloomFilter *filter, const char *str);

BloomFilter *BloomFilter_new(size_t size, size_t num_functions, ...) {
	va_list argp;

	BloomFilter *filter = malloc(sizeof(*filter));
    if (NULL==filter) {
        fprintf(stderr, "Out of memory.\n");
        exit(EXIT_FAILURE);
    }
	filter->num_items = 0;
	filter->bitv = createBitVector(size);
	filter->num_functions = num_functions;
	filter->hash_functions = malloc(sizeof(hash64_func)*num_functions);

    if (NULL==filter->hash_functions) {
        fprintf(stderr, "Out of memory.\n");
        exit(EXIT_FAILURE);
    }

	va_start(argp, num_functions);
    for(size_t i = 0; i < num_functions; i++) {
        filter->hash_functions[i] = va_arg(argp, hash64_func);
    }
	va_end(argp);
	return filter;
}

BloomFilter *BloomFilter_default(size_t size) {
	return BloomFilter_new(size, 1, hash_64);
}

void BloomFilter_free(BloomFilter *filter) {
    freeBitVector(filter->bitv);
    free(filter->hash_functions);
    free(filter);
}

void BloomFilter_put(BloomFilter *filter, const void *data, size_t size) {

	for (size_t i =0; i < filter->num_functions; i++) {
		uint64_t hash_val = filter->hash_functions[i](data, size);
		setBit(filter->bitv, hash_val % filter->bitv->size);
	}
	filter->num_items++;
}

void BloomFilter_putStr(BloomFilter *filter, const char *str) {
	BloomFilter_put(filter, str, strlen(str));
}

bool BloomFilter_exists(BloomFilter *filter, const void *data, size_t size) {
	for (size_t i = 0; i < filter->num_functions; i++) {
		uint64_t hash_val = filter->hash_functions[i](data, size);
		if (!getBit(filter->bitv, hash_val % filter->bitv->size)) {
			return false;
		}
	}
	return true;
}

bool BloomFilter_strExists(BloomFilter *filter, const char *str) {
	return BloomFilter_exists(filter, str, strlen(str));
}

#endif
