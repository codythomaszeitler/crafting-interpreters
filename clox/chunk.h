#ifndef CHUNK_HEADER
#define CHUNK_HEADER

#include "value.h"

typedef enum {
    OP_RETURN,
    OP_CONSTANT, 
    OP_NEGATE
} OpCode;

typedef struct Chunk {
    uint8_t* code;  
    int count;
    int capacity;
    ValueArray constants;
} Chunk;

void initChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t value);
void freeChunk(Chunk* chunk);

int addConstant(Chunk* chunk, Value constant);
Value getConstantAt(Chunk* chunk, int index);

#endif