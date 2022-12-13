#include <stdio.h>
#include <stdint.h>
#include "chunk.h"
#include "memory.h"
#include "value.h"
#include <string.h>

void initChunk(Chunk *chunk)
{
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;

    initValueArray(&chunk->constants);
}

void writeChunk(Chunk *chunk, uint8_t value)
{
    if (chunk->capacity < chunk->count + 1)
    {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }
    chunk->code[chunk->count] = value;
    chunk->count++;
}

void freeChunk(Chunk *chunk)
{
    FREE_ARRAY(uint8_t, chunk->code, chunk->count);
    initChunk(chunk);
}

int addConstant(Chunk *chunk, Value constant)
{
    int index = writeValueArray(&chunk->constants, constant);
    return index;
}

Value getConstantAt(Chunk *chunk, int index)
{
    return getValueAt(&chunk->constants, index);
}

uint8_t getByteLengthFor(OpCode opCode)
{
    uint8_t byteLength = 0;
    if (opCode == OP_MULT || opCode == OP_ADD || opCode == OP_DIV || opCode == OP_SUB || opCode == OP_TRUE || opCode == OP_FALSE || opCode == OP_EQUAL)
    {
        byteLength = 1;
    }
    else if (opCode == OP_CONSTANT)
    {
        byteLength = 2;
    }

    return byteLength;
}

void writeString(Chunk *chunk, const char *chars)
{
    int length = strlen(chars);

    for (int i = 0; i < length; i++)
    {
        char character = chars[i];
        writeChunk(chunk, character);
    }
    writeChunk(chunk, '\0');
}