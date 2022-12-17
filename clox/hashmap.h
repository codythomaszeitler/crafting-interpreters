#ifndef HASHMAP_HEADER
#define HASHMAP_HEADER

#include "value.h"
#include "cloxstring.h"

#define _NUM_HASH_BUCKETS_ 256

typedef struct Entry Entry;
struct Entry {
    StringObj* key;
    Value value;
    Entry* next;
};

typedef struct HashMap {
    Entry* entries[_NUM_HASH_BUCKETS_];
} HashMap;

void initHashMap(HashMap*);
void freeHashMap(HashMap*);
void hashMapPut(HashMap*, StringObj*, Value);
Value hashMapGet(HashMap*, StringObj*);
int hashMapSize(HashMap*);

#endif