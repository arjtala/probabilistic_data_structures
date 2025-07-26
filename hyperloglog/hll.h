#ifndef HLL_H
#define HLL_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "../lib/hash.h"

typedef struct {
	uint8_t *registers;
	hash64_func hash_function;
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
double HLL_count(HLL *hll);
void HLL_merge(HLL *hll_a, HLL *hll_b);
const int NUM_BITS_PER_REGISTER = 6;
HLL *HLL_new(size_t p, ...) {

	va_list argp;

	const size_t size = 8*sizeof(uint64_t);
	if (p < 4 || p > size) {
		fprintf(stderr, "Invalid parameter 4 < %zu < p=%zu", p, size);
		exit(EXIT_FAILURE);
	}

	HLL *hll = malloc(sizeof(*hll));
	if (NULL==hll) {
        fprintf(stderr, "Out of memory.\n");
        exit(EXIT_FAILURE);
	}
	hll->m = (1UL << p); // m = 2^p
	hll->p = p;
	hll->q = size - p;
	hll->num_bits_per_register = NUM_BITS_PER_REGISTER; // 6 bits covers up to 64 (more than enough)

	hll->registers = calloc(hll->m, sizeof(uint8_t));
	if (!hll->registers) {
        fprintf(stderr, "Out of memory.\n");
        exit(EXIT_FAILURE);
	}
	va_start(argp, p);
    hll->hash_function = va_arg(argp, hash64_func);
	va_end(argp);

	return hll;
}

HLL *HLL_default(size_t p) {
	return HLL_new(p, murmur64);
}

void freeHLL(HLL *hll) {
	free(hll->registers);
	free(hll);
}

size_t HLL_memory_usage(const HLL *hll) {
    if (!hll) {
        fprintf(stderr, "Hyperloglog not intialized.\n");
        exit(EXIT_FAILURE);
	}

    size_t static_size = sizeof(HLL);                         // struct itself
    size_t registers_size = hll->m * sizeof(uint8_t);         // register array
    return static_size + registers_size;
}

void HLL_add(HLL *hll, const void *data, size_t size) {
	if (!hll) {
        fprintf(stderr, "Hyperloglog not intialized.\n");
        exit(EXIT_FAILURE);
	}
	if (!data) {
        fprintf(stderr, "No data provided.\n");
		return;
	}
	const size_t hash_size = 8 * sizeof(uint64_t);

	uint64_t hash_val = hll->hash_function(data, size);

	// j = 1 + <x_1 x_2 ... x_b>_2
    // Extract the first p bits and add 1
	uint64_t j = hash_val >> (hash_size - hll->p); // Now j ∈ [0, m-1]
	if (j >= hll->m) {
		fprintf(stderr, "BUG: j out of range: %llu\n", (unsigned long long)j);
		exit(1);
	}

	// w = x_{b+1} x_{b+2} ...
    // Extract the remaining q bits
	uint64_t w = hash_val << hll->p;

	// M[j] = max(M[j], p(w))
    size_t p_w = msb_position(w, hll->q);

	// Ensure p_w fits in our register size
    if (p_w > (1ULL << hll->num_bits_per_register) - 1) {
        p_w = (1ULL << hll->num_bits_per_register) - 1;
    }

	// Update register with maximum
	if (p_w > hll->registers[j]) {
		hll->registers[j] = (uint8_t)p_w;
	}
}

// Helper function to compute the bias correction constant a_m
static double get_alpha_m(size_t m) {
    switch (m) {
        case 16:
            return 0.673;
        case 32:
            return 0.697;
        case 64:
            return 0.709;
        default:
            if (m >= 128) {
                return 0.7213 / (1.0 + 1.079 / (double)m);
            } else {
                // For m < 16, use the formula (though not recommended)
                return 0.7213 / (1.0 + 1.079 / (double)m);
            }
    }
}

double HLL_count(HLL *hll) {
    if (!hll) {
        return 0.0;
    }


    double alpha_m = get_alpha_m(hll->m);
    double sum = 0.0;
    size_t zero_count = 0;

    for (size_t i = 0; i < hll->m; ++i) {
        uint8_t reg = hll->registers[i];
        sum += 1.0 / (1ULL << reg); // 2^(-register value)
        if (reg == 0) {
            zero_count++;
        }
    }

    double raw_estimate = alpha_m * hll->m * hll->m / sum;

    // Small range correction: linear counting
    if (raw_estimate <= (5.0 / 2.0) * hll->m && zero_count > 0) {
        return hll->m * log((double)hll->m / zero_count);
    }

    // Large range correction (optional but common in full implementations)
    const double two_to_64 = 18446744073709551616.0; // 2^64
    if (raw_estimate > (1.0 / 30.0) * two_to_64) {
        return -two_to_64 * log(1.0 - (raw_estimate / two_to_64));
    }

    return raw_estimate;
}

HLL *HLL_merge_copy(const HLL *a, const HLL *b) {
    if (!a || !b) {
        fprintf(stderr, "Error: One or both HLL inputs are NULL.\n");
        return NULL;
    }

    if (a->p != b->p || a->m != b->m) {
        fprintf(stderr, "Error: HLLs have incompatible precision or size.\n");
        return NULL;
    }

    if (a->hash_function != b->hash_function) {
        fprintf(stderr, "Warning: HLLs use different hash functions. Proceeding anyway.\n");
    }

    // Create a new HLL instance with the same parameters
    HLL *merged = HLL_new(a->p, a->hash_function);
    if (!merged) {
        fprintf(stderr, "Error: Failed to allocate merged HLL.\n");
        return NULL;
    }

    // Take the element-wise maximum of the registers
    for (size_t i = 0; i < merged->m; ++i) {
        merged->registers[i] = (a->registers[i] > b->registers[i]) ?
                                a->registers[i] : b->registers[i];
    }

    return merged;
}

#endif
