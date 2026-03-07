#include "hash.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint64_t djb2(const void *buf, size_t length) {
  uint64_t hash = DJB2_INIT;
  const uint8_t *data = (const uint8_t *)buf;
  for (size_t i = 0; i < length; i++) {
    hash = ((hash << 5) + hash) + data[i];
  }
  return hash;
}

uint64_t sdbm(const void *buf, size_t length) {
  uint64_t hash = 0;
  const uint8_t *data = (const uint8_t *)buf;
  for (size_t i = 0; i < length; i++) {
    hash = data[i] + (hash << 6) + (hash << 16) - hash;
  }
  return hash;
}

// FNV-1a
uint64_t hash_64(const void *buf, size_t len) {
  uint64_t seed = 14695981039346656037ULL;
  uint64_t prime = 1099511628211ULL;
  const uint8_t *data = (const uint8_t *)buf;
  uint64_t h = seed;
  for (size_t i = 0; i < len; ++i) {
    h ^= data[i];
    h *= prime;
  }
  return h;
}

// FNV-1
uint64_t fnv_64(const void *buf, size_t len, uint64_t hval) {
  unsigned char *bp = (unsigned char *)buf; /* start of buffer */
  unsigned char *be = bp + len;             /* beyond end of buffer */

  /* FNV-1 hash each octet of the buffer */
  while (bp < be) {
    /* multiply by the 64 bit FNV magic prime mod 2^64 */
    hval += (hval << 1) + (hval << 4) + (hval << 5) + (hval << 7) +
            (hval << 8) + (hval << 40);
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
  uint64_t k1, k2;
  for (int i = 0; i < nblocks; i++) {
    memcpy(&k1, data + i * 16, 8);
    memcpy(&k2, data + i * 16 + 8, 8);

    k1 *= c1;
    k1 = (k1 << 31) | (k1 >> (64 - 31));
    k1 *= c2;
    h1 ^= k1;
    h1 = (h1 << 27) | (h1 >> (64 - 27));
    h1 += h2;
    h1 = h1 * 5 + 0x52dce729;

    k2 *= c2;
    k2 = (k2 << 33) | (k2 >> (64 - 33));
    k2 *= c1;
    h2 ^= k2;
    h2 = (h2 << 31) | (h2 >> (64 - 31));
    h2 += h1;
    h2 = h2 * 5 + 0x38495ab5;
  }

  // Tail
  const uint8_t *tail = (const uint8_t *)(data + nblocks * 16);
  k1 = 0;
  k2 = 0;

  switch (len & 15) {
  case 15:
    k2 ^= ((uint64_t)tail[14]) << 48;
  case 14:
    k2 ^= ((uint64_t)tail[13]) << 40;
  case 13:
    k2 ^= ((uint64_t)tail[12]) << 32;
  case 12:
    k2 ^= ((uint64_t)tail[11]) << 24;
  case 11:
    k2 ^= ((uint64_t)tail[10]) << 16;
  case 10:
    k2 ^= ((uint64_t)tail[9]) << 8;
  case 9:
    k2 ^= ((uint64_t)tail[8]) << 0;
    k2 *= c2;
    k2 = (k2 << 33) | (k2 >> (64 - 33));
    k2 *= c1;
    h2 ^= k2;
  case 8:
    k1 ^= ((uint64_t)tail[7]) << 56;
  case 7:
    k1 ^= ((uint64_t)tail[6]) << 48;
  case 6:
    k1 ^= ((uint64_t)tail[5]) << 40;
  case 5:
    k1 ^= ((uint64_t)tail[4]) << 32;
  case 4:
    k1 ^= ((uint64_t)tail[3]) << 24;
  case 3:
    k1 ^= ((uint64_t)tail[2]) << 16;
  case 2:
    k1 ^= ((uint64_t)tail[1]) << 8;
  case 1:
    k1 ^= ((uint64_t)tail[0]) << 0;
    k1 *= c1;
    k1 = (k1 << 31) | (k1 >> (64 - 31));
    k1 *= c2;
    h1 ^= k1;
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

HashTable *HashTable_create(void (*free_value)(void *)) {
  HashTable *ht = malloc(sizeof(HashTable));
  if (NULL == ht) {
    fprintf(stderr, "Unable to initialize HashTable: out of memory.\n");
    exit(EXIT_FAILURE);
  }
  ht->length = 0;
  ht->capacity = INITIAL_HASH_TABLE_CAPACITY;
  ht->entries = calloc(ht->capacity, sizeof(HashTableEntry));
  if (NULL == ht->entries) {
    fprintf(stderr, "Unable to initialize HashTableEntries: out of memory.\n");
    free(ht);
    exit(EXIT_FAILURE);
  }
  ht->free_value = free_value;
  return ht;
}

void HashTable_free(HashTable *ht) {
  for (size_t i = 0; i < ht->capacity; ++i) {
    if (ht->entries[i].key != NULL) {
      free((void*)ht->entries[i].key);
      if (ht->free_value)
        ht->free_value(ht->entries[i].value);
    }
  }
  free(ht->entries);
  free(ht);
}

void *HashTable_get(HashTable *ht, const char *key) {
  uint64_t hash = murmur64(key, strlen(key), DEFAULT_MURMUR64_KEY);
  size_t index = (size_t)(hash & (uint64_t)(ht->capacity - 1));
  // Loop until we find an empty entry
  while (ht->entries[index].key != NULL) {
    if (strcmp(key, ht->entries[index].key) == 0) {
      // Found key, return value
      return ht->entries[index].value;
    }
    // Key not in slot, move to next (linear probing)
    index++;
    // At end of entries array, wrap around
    if (index >= ht->capacity)
      index = 0;
  }
  return NULL;
}

// Internal function to set an entry without expanding the table
static const char *HashTable_set_entry(HashTableEntry *entries, size_t capacity,
                                       const char *key, void *value,
                                       size_t *plength) {
  // AND hash with capacity - 1 to ensure it's within entries array
  uint64_t hash = murmur64(key, strlen(key), DEFAULT_MURMUR64_KEY);
  size_t index = (size_t)(hash & (uint64_t)(capacity - 1));

  // Loop till we find an empty slot
  while (entries[index].key != NULL) {
    if (strcmp(key, entries[index].key) == 0) {
      // Found key (already exists), update value
      entries[index].value = value;
      return entries[index].key;
    }
    // Key not in slot, move to next (linear probing)
    index++;
    if (index >= capacity) {
      index = 0;
    }
  }
  // Not found, allocate + copy if necessary
  if (plength != NULL) {
    key = strdup(key);
    if (key == NULL) {
      return NULL;
    }
    (*plength)++;
  }
  entries[index].key = (char *)key;
  entries[index].value = value;
  return key;
}

static bool HashTable_expand(HashTable *ht) {
  // Allocate new entries
  size_t capacity = ht->capacity * 2;
  if (capacity < ht->capacity) {
    return false; // overflow (capacity would be too big)
  }
  HashTableEntry *entries = calloc(capacity, sizeof(HashTableEntry));
  if (entries == NULL) {
    return false;
  }
  // Iterate entries and move all non-empty ones to new table
  for (size_t i = 0; i < ht->capacity; ++i) {
    HashTableEntry entry = ht->entries[i];
    if (entry.key != NULL) {
      HashTable_set_entry(entries, capacity, entry.key, entry.value, NULL);
    }
  }
  free(ht->entries);
  ht->entries = entries;
  ht->capacity = capacity;
  return true;
}

const char *HashTable_set(HashTable *ht, const char *key, void *value) {
  assert(value != NULL);

  if (ht->length >= ht->capacity / 2) {
    if (!HashTable_expand(ht)) {
      return NULL;
    }
  }
  // Set entry and update length
  return HashTable_set_entry(ht->entries, ht->capacity, key, value,
                             &ht->length);
}

size_t HashTable_size(HashTable *ht) { return ht->length; }

HashTableIterator HashTable_iterator(HashTable *ht) {
  HashTableIterator it;
  it._table = ht;
  it._index = 0;
  return it;
}

bool HashTable_next(HashTableIterator *it) {
  // Loop until end of entities
  HashTable *ht = it->_table;
  while (it->_index < ht->capacity) {
    size_t i = it->_index;
    it->_index++;
    if (ht->entries[i].key != NULL) {
      // Next non-empty item, update iterator key and value
      HashTableEntry entry = ht->entries[i];
      it->key = entry.key;
      it->value = entry.value;
      return true;
    }
  }
  return false;
}
