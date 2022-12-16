#include "bsa.h"
#include "hashmap.h"
#include "value.h"
#include "memory.h"
#include "stdlib.h"

void initBlockStaticAnalysis(BlockStaticAnalysis *block)
{
    block->parent = NULL; 
    block->count = 0;
    block->capacity = 0;
    block->blocks = NULL;
    initHashMap(&block->bindings);
}

void freeBlockStaticAnalysis(BlockStaticAnalysis *block)
{
    for (int i = 0; i < block->count; i++) {
        freeBlockStaticAnalysis(block->blocks[i]);
    }

    freeHashMap(&block->bindings);
    free(block->blocks);
    free(block);
}

void addBlockTo(BlockStaticAnalysis *target, BlockStaticAnalysis *block)
{
    if (target->capacity < target->count + 1)
    {
        int oldCount = target->capacity;
        target->capacity = GROW_CAPACITY(target->capacity);
        target->blocks = GROW_ARRAY(BlockStaticAnalysis*, target->blocks, oldCount, target->capacity);
    }

    int constantIndex = target->count;
    target->blocks[constantIndex] = block;
    target->count = constantIndex + 1;
    block->parent = target;
}

void addLocalVariableBinding(BlockStaticAnalysis *target, const char *variableName, int topOffsetStackLocation)
{
    hashMapPut(&target->bindings, variableName, wrapNumber((double)topOffsetStackLocation));
}

int getLocalVariableBinding(BlockStaticAnalysis* target, const char* variableName) {
    Value offsetAsValue = hashMapGet(&target->bindings, variableName);
    int offsetAsInt = (int) unwrapNumber(offsetAsValue);
    return offsetAsInt;
}