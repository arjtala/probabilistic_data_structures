#ifndef HASHF_H
#define HASHF_H

#include <stdint.h>
#define DJB2_INIT 5381
#define HASH64_NUM_FUNCTIONS 2
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

#include <stdlib.h>
#include <stdint.h>

typedef uint64_t (*hash64_func)(const void *data, size_t length);
uint64_t djb2(const void *buff, size_t length);
uint64_t sdbm(const void *buff, size_t length);
uint64_t hash_64(const void *buff, size_t len);
uint64_t fnv_64(void *buf, size_t len, uint64_t hval);
uint64_t murmur64(const void *key, size_t len, uint64_t seed);

#endif
