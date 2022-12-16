#ifndef HASHMAP_HEADER
#define HASHMAP_HEADER

#include "value.h"

#define _NUM_HASH_BUCKETS_ 256

typedef struct Entry Entry;
struct Entry {
    const char* key;
    Value value;
    Entry* next;
};

typedef struct HashMap {
    Entry* entries[_NUM_HASH_BUCKETS_];
} HashMap;

void initHashMap(HashMap*);
void freeHashMap(HashMap*);
void hashMapPut(HashMap*, const char*, Value);
Value hashMapGet(HashMap*, const char*);
int hashMapSize(HashMap*);

#endif