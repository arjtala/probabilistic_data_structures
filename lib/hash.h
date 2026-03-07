#ifndef HASHF_H
#define HASHF_H

#include <stdbool.h>
#include <stdint.h>

#define DJB2_INIT 5381
#define HASH64_NUM_FUNCTIONS 2
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL
#define INITIAL_HASH_TABLE_CAPACITY 16
#define DEFAULT_MURMUR64_KEY 42

#include <stdlib.h>
#include <stdint.h>

typedef uint64_t (*hash64_func)(const void *data, size_t length);
uint64_t djb2(const void *buff, size_t length);
uint64_t sdbm(const void *buff, size_t length);
uint64_t hash_64(const void *buff, size_t len);
uint64_t fnv_64(void *buf, size_t len, uint64_t hval);
uint64_t murmur64(const void *key, size_t len, uint64_t seed);

typedef struct HashTable HashTable;
typedef struct {     // HashTable iterator
  const char* key;   // current key
  void* value;       // current value
  // private
  HashTable *_table; // Reference to HashTable being itereated
  size_t _index;      // Current index into ht._entries;
} HashTableIterator;
typedef struct {
  const char *key;
  void *value;
} HashTableEntry;
struct HashTable {
  HashTableEntry *entries;
  size_t capacity;
  size_t length;
};
HashTable *HashTable_create(void);
void HashTable_free(HashTable *ht);
void *HashTable_get(HashTable *ht, const char *key);
const char *HashTable_set(HashTable *ht, const char *key, void *value);
size_t HashTable_size(HashTable *ht);
HashTableIterator HashTable_iterator(HashTable *ht);
bool HashTable_next(HashTableIterator *hti);

#endif
