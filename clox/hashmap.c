#include "hashmap.h"
#include "value.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include <math.h>
#include "cloxstring.h"

unsigned long hash(const unsigned char *str);

void initHashMap(HashMap *map)
{
    for (int i = 0; i < _NUM_HASH_BUCKETS_; i++)
    {
        map->entries[i] = NULL;
    }
}

void freeHashMap(HashMap *map)
{
    for (int i = 0; i < _NUM_HASH_BUCKETS_; i++)
    {
        Entry *iterator = map->entries[i];
        while (iterator != NULL)
        {
            Entry *next = iterator->next;
            free(iterator);
            iterator = next;
        }
    }
}

void hashMapPut(HashMap *map, StringObj *key, Value value)
{
    unsigned long hashed = hash((const unsigned char *)key->chars);
    unsigned long bucket = hashed % _NUM_HASH_BUCKETS_;

    Entry *iterator = map->entries[bucket];
    if (iterator == NULL)
    {
        map->entries[bucket] = iterator = malloc(sizeof(Entry));
        iterator->key = key;
        iterator->value = value;
        iterator->next = NULL;
    }
    else
    {
        while (iterator != NULL)
        {
            if (!strcmp(iterator->key->chars, key->chars))
            {
                iterator->value = value;
                break;
            }
            else if (iterator->next == NULL)
            {
                Entry *newElement = malloc(sizeof(Entry));

                iterator->next = newElement;
                newElement->key = key;
                newElement->value = value;
                newElement->next = NULL;
            }
            iterator = iterator->next;
        }
    }
}

Value hashMapGet(HashMap *map, StringObj* key)
{
    unsigned long hashed = hash((const unsigned char *)key->chars);
    unsigned long bucket = hashed % _NUM_HASH_BUCKETS_;

    Entry *iterator = map->entries[bucket];
    if (iterator == NULL)
    {
        return nil();
    }

    while (iterator != NULL)
    {
        if (!strcmp(iterator->key->chars, key->chars))
        {
            return iterator->value;
        }
        iterator = iterator->next;
    }

    return nil();
}

unsigned long hash(const unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

static int getNumElements(Entry *);

int hashMapSize(HashMap *map)
{
    int size = 0;
    for (int i = 0; i < _NUM_HASH_BUCKETS_; i++)
    {
        Entry *iterator = map->entries[i];
        size = getNumElements(iterator) + size;
    }
    return size;
}

static int getNumElements(Entry *entry)
{
    Entry *iterator = entry;
    int numElements = 0;
    while (iterator != NULL)
    {
        numElements = numElements + 1;
        iterator = iterator->next;
    }
    return numElements;
}