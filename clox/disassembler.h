#ifndef DISASSEMBLER_HEADER
#define DISASSEMBLER_HEADER

#include "chunk.h"

void disassembleChunk(Chunk* bytecode, const char* chunkName, void (*callback)(const char* message));

#endif