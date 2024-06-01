#ifndef HASHF_H
#define HASHF_H

#define DJB2_INIT 5381
#define HASH32_NUM_FUNCTIONS 2

#include <stdlib.h>
#include <stdint.h>

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

#endif
