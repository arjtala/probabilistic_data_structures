#ifndef HASHF_H
#define HASHF_H

#define DJB2_INIT 5381
#define HASH64_NUM_FUNCTIONS 2
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

#include <stdlib.h>
#include <stdint.h>

typedef uint64_t (*hash64_func)(const void *data, size_t length);

uint64_t djb2(const void *buff, size_t length) {
    uint64_t hash = DJB2_INIT;
    const uint8_t *data = buff;
    for(size_t i = 0; i < length; i++) {
         hash = ((hash << 5) + hash) + data[i];
    }
    return hash;
}

uint64_t sdbm(const void *buff, size_t length) {
	uint64_t hash = 0;
    const uint8_t *data = buff;
    for(size_t i = 0; i < length; i++) {
        hash = data[i] + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}


uint64_t hash_64(const void *buff, size_t len) {
	uint64_t seed = 14695981039346656037ULL;
	uint64_t prime = 1099511628211ULL;
    const uint8_t *data = buff;
    uint64_t h = seed;
    for (size_t i = 0; i < len; ++i) {
        h ^= data[i];
        h *= prime;
    }
    return h;
}

uint64_t fnv_64(void *buf, size_t len, uint64_t hval) {
    unsigned char *bp = (unsigned char *)buf;   /* start of buffer */
    unsigned char *be = bp + len;               /* beyond end of buffer */

    /* FNV-1 hash each octet of the buffer */
    while (bp < be) {
		/* multiply by the 64 bit FNV magic prime mod 2^64 */
		hval += (hval << 1) + (hval << 4) + (hval << 5) +
			(hval << 7) + (hval << 8) + (hval << 40);
		/* xor the bottom with the current octet */
		hval ^= (uint64_t)*bp++;
    }
    return hval;
}

uint64_t murmur64(const void *key, size_t len, uint64_t seed) {
    const uint8_t *data = (const uint8_t *)key;
    const int nblocks = len / 16;

    uint64_t h1 = seed;
    uint64_t h2 = seed;

    const uint64_t c1 = 0x87c37b91114253d5ULL;
    const uint64_t c2 = 0x4cf5ad432745937fULL;

    // Body
    const uint64_t *blocks = (const uint64_t *)(data);
    for (int i = 0; i < nblocks; i++) {
        uint64_t k1 = blocks[i * 2 + 0];
        uint64_t k2 = blocks[i * 2 + 1];

        k1 *= c1; k1 = (k1 << 31) | (k1 >> (64 - 31)); k1 *= c2; h1 ^= k1;
        h1 = (h1 << 27) | (h1 >> (64 - 27)); h1 += h2; h1 = h1 * 5 + 0x52dce729;

        k2 *= c2; k2 = (k2 << 33) | (k2 >> (64 - 33)); k2 *= c1; h2 ^= k2;
        h2 = (h2 << 31) | (h2 >> (64 - 31)); h2 += h1; h2 = h2 * 5 + 0x38495ab5;
    }

    // Tail
    const uint8_t *tail = (const uint8_t *)(data + nblocks * 16);
    uint64_t k1 = 0;
    uint64_t k2 = 0;

    switch (len & 15) {
        case 15: k2 ^= ((uint64_t)tail[14]) << 48;
        case 14: k2 ^= ((uint64_t)tail[13]) << 40;
        case 13: k2 ^= ((uint64_t)tail[12]) << 32;
        case 12: k2 ^= ((uint64_t)tail[11]) << 24;
        case 11: k2 ^= ((uint64_t)tail[10]) << 16;
        case 10: k2 ^= ((uint64_t)tail[ 9]) << 8;
        case  9: k2 ^= ((uint64_t)tail[ 8]) << 0;
                 k2 *= c2; k2 = (k2 << 33) | (k2 >> (64 - 33)); k2 *= c1; h2 ^= k2;
        case  8: k1 ^= ((uint64_t)tail[ 7]) << 56;
        case  7: k1 ^= ((uint64_t)tail[ 6]) << 48;
        case  6: k1 ^= ((uint64_t)tail[ 5]) << 40;
        case  5: k1 ^= ((uint64_t)tail[ 4]) << 32;
        case  4: k1 ^= ((uint64_t)tail[ 3]) << 24;
        case  3: k1 ^= ((uint64_t)tail[ 2]) << 16;
        case  2: k1 ^= ((uint64_t)tail[ 1]) << 8;
        case  1: k1 ^= ((uint64_t)tail[ 0]) << 0;
                 k1 *= c1; k1 = (k1 << 31) | (k1 >> (64 - 31)); k1 *= c2; h1 ^= k1;
    }

    // Finalization
    h1 ^= len;
    h2 ^= len;

    h1 += h2;
    h2 += h1;

    // fmix64
    h1 ^= h1 >> 33;
    h1 *= 0xff51afd7ed558ccdULL;
    h1 ^= h1 >> 33;
    h1 *= 0xc4ceb9fe1a85ec53ULL;
    h1 ^= h1 >> 33;

    return h1;
}

#endif
