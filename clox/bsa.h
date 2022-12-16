#ifndef BSA_HEADER
#define BSA
#include "hashmap.h"

typedef struct BlockStaticAnalysis {
    struct BlockStaticAnalysis** blocks;
    int count;
    int capacity;
    struct BlockStaticAnalysis* parent;
    HashMap bindings;
} BlockStaticAnalysis;

typedef struct LocalVariableBinding {
    const char* name;
    int topOffsetStackLocation; 
} LocalVariableBinding;

void initBlockStaticAnalysis(BlockStaticAnalysis*);
void freeBlockStaticAnalysis(BlockStaticAnalysis*);
void addBlockTo(BlockStaticAnalysis* target, BlockStaticAnalysis*block);
void addLocalVariableBinding(BlockStaticAnalysis* target, const char* variableName, int topOffsetStackLocation);
int getLocalVariableBinding(BlockStaticAnalysis* target, const char* variableName);

#endif