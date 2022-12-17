#ifndef CHUNK_HEADER
#define CHUNK_HEADER

#include "value.h"

typedef enum {
    OP_RETURN,
    OP_CONSTANT, 
    OP_NEGATE,
    OP_ADD, 
    OP_MULT,
    OP_DIV,
    OP_SUB,
    OP_TRUE,
    OP_FALSE,
    OP_EQUAL,
    OP_STRING,
    OP_PRINT,
    OP_VAR_DECL,
    OP_VAR_ASSIGN,
    OP_VAR_EXPRESSION,
    OP_STACK_PEEK,
    OP_POP
} OpCode;

uint8_t getByteLengthFor(OpCode opCode);

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

void writeString(Chunk* chunk, const char* string);

#endif