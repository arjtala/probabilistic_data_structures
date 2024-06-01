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
	BitVector *registers;
	hash32_func *hash_functions;
	size_t num_bits_per_register;
	size_t num_functions;
	size_t p; // Precision parameter that controls relative estimation error
	size_t m;
} HLL;

HLL *HLL_new(size_t p, size_t num_bits_per_register, size_t num_functions, ...);
HLL *HLL_default(size_t p);
void freeHLL(HLL *hll);
void HLLad(HLL *hll);
void HLLcount(HLL *hll);
void HLLmerge(HLL *hll_a, HLL *hll_b);

HLL *HLL_new(size_t p, size_t num_bits_per_register, size_t num_functions, ...) {
	va_list argp;

	HLL *hll = malloc(sizeof(*hll));
	if (NULL==hll) {
        fprintf(stderr, "Out of memory.\n");
        exit(EXIT_FAILURE);
	}
	hll->m = (1 << p);
	hll->registers = malloc(hll->m * sizeof(BitVector));
	for (size_t i = 0; i < hll->m; i++) {
		hll->registers[i] = *createBitVector(num_bits_per_register);
	}

	va_start(argp, num_functions);
    for(size_t i = 0; i < num_functions; i++) {
        hll->hash_functions[i] = va_arg(argp, hash32_func);
    }
	va_end(argp);

	return hll;
}

#endif
