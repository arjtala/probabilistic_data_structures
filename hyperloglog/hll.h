#ifndef HLL_H
#define HLL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "hash.h"
#include "bitvector.h"

typedef struct {
	uint8_t *registers;
	hash32_func hash_function;
	size_t num_bits_per_register;
	size_t num_functions;
	size_t p; // Precision parameter that controls relative estimation error
	size_t q; // Using a (p+q)-bit hash value
	size_t m; // Number of registers
} HLL;

HLL *HLL_new(size_t p, ...);
HLL *HLL_default(size_t p);
void freeHLL(HLL *hll);
void HLL_add(HLL *hll, const void *data, size_t size);
void HLL_count(HLL *hll);
void HLL_merge(HLL *hll_a, HLL *hll_b);

HLL *HLL_new(size_t p, ...) {

	printf("Initializing HLL...\n");

	va_list argp;

	const size_t size = 8*sizeof(uint64_t);
	HLL *hll = malloc(sizeof(*hll));
	if (NULL==hll) {
        fprintf(stderr, "Out of memory.\n");
        exit(EXIT_FAILURE);
	}
	hll->m = (1 << p);
	hll->p = p;
	hll->q = size - p;

	printf("Initializing %zu registers\n", hll->m);
	hll->registers = malloc(hll->m * sizeof(uint8_t));
	for (size_t i = 0; i < hll->m; i++) {
		hll->registers[i] = 0;
	}
	va_start(argp, p);

	printf("Setting hashing function\n");
    hll->hash_function = va_arg(argp, hash32_func);
	va_end(argp);

	return hll;
}

HLL *HLL_default(size_t p) {
	return HLL_new(p, hash_64);
}

void freeHLL(HLL *hll) {
	free(hll->registers);
	free(hll);
}

void HLL_add(HLL *hll, const void *data, size_t size) {
	printf("Hashing input\n");
	uint64_t hash_val = hll->hash_function(data, size);

	uint64_t a = (hash_val >> hll->q) & ((1ULL << hll->p) - 1);
	uint64_t b = hash_val & ((1ULL << hll->q) - 1);

	int offset = 8*sizeof(uint64_t);
	printf("Offset: %d\n", offset);
	int k = msb_position(b, offset);
	if (k < 0) {
		k = hll->q+1;
	}

	printf("a: %llu\n", a);
	printBinary(a, offset);
	printf("b: %llu\n", b);
	printBinary(b, offset);

    /* int k = hll->q + 1; */
    /* while (k > 0 && !((hash_val >> (k - 1)) & 1)) { */
    /*     k--; */
    /* } */
    int i = 1 + a;
	// int i = hash_val & (hll->m - 1);

	printf("%llu [%d]:\n", hash_val, k);
	printBinary(hash_val, offset);
	for (int _c = 0; _c < k; _c++) {
		if (_c < hll->p) {
			printf("*");
		} else if (_c == hll->p) {
			printf("|");
		} else {
			printf("_");
		}
	}
	printf("^\n");

	printf("Parameters: hash=%llu, a=%llu, b=%llu, p=%zu, q=%zu, k=%d, i=%d\n", hash_val, a, b, hll->p, hll->q, k, i);

	printf("Updating register[%d]: %d < %d: ", i, hll->registers[i], k);
	if (hll->registers[i] < k) {
		printf("true\n");
		hll->registers[i] = k;
	} else {printf("false\n");}
}

#endif
