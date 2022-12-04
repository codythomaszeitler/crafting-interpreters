#include "disassembler.h"
#include "chunk.h"
#include "stdint.h"
#include "stdio.h"
#include <stdlib.h>

uint8_t disassemble_peek(Chunk* bytecode, int index);
uint8_t disassembleInstruction(Chunk* bytecode, int index, void (*logger)(char* message));

void disassembleChunk(Chunk* bytecode, const char* chunkName, void (*callback)(char* message)) {
    int iterator = 0;

    int length = snprintf(NULL, 0, "=== %s ===\n", chunkName);
    char *line = malloc(sizeof(char) * length);
    snprintf(line, length + 1, "=== %s ===\n", chunkName);
    callback(line);

    while (iterator < bytecode->count) {
        iterator = iterator + disassembleInstruction(bytecode, iterator, callback);
    }
}

uint8_t disassemble_peek(Chunk* bytecode, int index) {
    uint8_t byte = bytecode->code[index];
    return byte;
}

uint8_t disassembleInstruction(Chunk* bytecode, int index, void (*logger)(char* message)) {
    OpCode opCode = disassemble_peek(bytecode, index);

    if (opCode == OP_RETURN) {
        const char* opCodeAsString = "OP_RETURN";
        int length = snprintf(NULL, 0, "%04d %s\n", index, opCodeAsString);

        char *line = malloc(sizeof(char) * length);
        snprintf(line, length + 1, "%04d %s\n", index, opCodeAsString);

        logger(line);

        return 1;
    } else if (opCode == OP_CONSTANT) {
        uint8_t indexConstant = bytecode->code[index + 1];
        Value constant = getConstantAt(bytecode, indexConstant);

        const char* opCodeAsString = "OP_CONSTANT";
        int length = snprintf(NULL, 0, "%04d %s %i %f\n", index, opCodeAsString, indexConstant, constant);

        char *line = malloc(sizeof(char) * length);
        snprintf(line, length + 1, "%04d %s %i %f\n", index, opCodeAsString, indexConstant, constant);

        logger(line);

        return 2;
    }

    return -1;
}