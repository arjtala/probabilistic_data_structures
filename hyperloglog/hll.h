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
#include "../lib/bitarray.h"

#define NUM_BITS_PER_REGISTER 6

typedef struct {
  uint8_t *registers;
  hash64_func hash_function;
  size_t num_bits_per_register;
  size_t p;  // Precision parameter that controls relative estimation error
  size_t q;  // Using a (p+q)-bit hash value
  size_t m;  // Number of registers
} HLL;

HLL *HLL_new(size_t p, ...);
HLL *HLL_default(size_t p);
void freeHLL(HLL *hll);
void HLL_add(HLL *hll, const void *data, size_t size);
double HLL_count(HLL *hll);
void HLL_merge(HLL *hll_a, HLL *hll_b);
HLL *HLL_merge_copy(const HLL *a, const HLL *b);
size_t HLL_memory_usage(const HLL *hll);

#endif
