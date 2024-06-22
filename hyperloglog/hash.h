#ifndef HASHF_H
#define HASHF_H

#include <stdlib.h>
#include <stdint.h>

#define DJB2_INIT 5381
#define HASH32_NUM_FUNCTIONS 2
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

typedef uint32_t (*hash32_func)(const void *data, size_t length);

uint32_t djb2(const void *buff, size_t length) {
    uint32_t hash = DJB2_INIT;
    const uint8_t *data = buff;
    for(size_t i = 0; i < length; i++) {
         hash = ((hash << 5) + hash) + data[i];
    }
    return hash;
}

uint32_t sdbm(const void *buff, size_t length) {
	uint32_t hash = 0;
    const uint8_t *data = buff;
    for(size_t i = 0; i < length; i++) {
        hash = data[i] + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}

uint64_t hash_64(const void *buff, size_t length) {
    const uint8_t *data = (const uint8_t *)buff;
    uint64_t hash = 0xCBF29CE484222325; // A prime number as initial seed
    uint64_t prime = 0x100000001B3;     // A different prime number for multiplication
    for (size_t i = 0; i < length; ++i) {
        hash ^= data[i];                // XOR byte into hash
        hash *= prime;                  // Multiply by prime
    }
    return hash;
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


#endif
