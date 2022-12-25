#include "disassembler.h"
#include "chunk.h"
#include "stdint.h"
#include "stdio.h"
#include <stdlib.h>
#include <stdarg.h>

uint8_t disassemble_peek(Chunk *bytecode, int index);
uint8_t disassembleInstruction(Chunk *bytecode, int index, void (*logger)(char *message));

void disassembleChunk(Chunk *bytecode, const char *chunkName, void (*callback)(char *message))
{
    int iterator = 0;

    int length = snprintf(NULL, 0, "=== %s ===\n", chunkName);
    char *line = malloc(sizeof(char) * length + 1);
    snprintf(line, length + 1, "=== %s ===\n", chunkName);
    callback(line);

    while (iterator < bytecode->count)
    {
        iterator = iterator + disassembleInstruction(bytecode, iterator, callback);
    }
}

uint8_t disassemble_peek(Chunk *bytecode, int index)
{
    uint8_t byte = bytecode->code[index];
    return byte;
}

uint8_t disassembleInstruction(Chunk *bytecode, int index, void (*logger)(char *message))
{
    OpCode opCode = disassemble_peek(bytecode, index);

    if (opCode == OP_RETURN)
    {
        const char *opCodeAsString = "OP_RETURN";
        int length = snprintf(NULL, 0, "%04d %s\n", index, opCodeAsString);

        char *line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%04d %s\n", index, opCodeAsString);

        logger(line);

        return 1;
    }
    else if (opCode == OP_CONSTANT)
    {
        uint8_t indexConstant = bytecode->code[index + 1];
        Value constant = getConstantAt(bytecode, indexConstant);

        const char *opCodeAsString = "OP_CONSTANT";
        int length = snprintf(NULL, 0, "%04d %s %i %f\n", index, opCodeAsString, indexConstant, unwrapNumber(constant));

        char *line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%04d %s %i %f\n", index, opCodeAsString, indexConstant, unwrapNumber(constant));

        logger(line);

        return 2;
    }
    else if (opCode == OP_NEGATE)
    {
        const char *opCodeAsString = "OP_NEGATE";
        int length = snprintf(NULL, 0, "%04d %s\n", index, opCodeAsString);

        char *line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%04d %s\n", index, opCodeAsString);

        logger(line);

        return 1;
    }
    else if (opCode == OP_STACK_PEEK)
    {
        const char *opCodeAsString = "OP_STACK_PRINT";
        int length = snprintf(NULL, 0, "%04d %s\n", index, opCodeAsString);
        char *line = malloc(sizeof(char) * length + 1);

        snprintf(line, length + 1, "%04d %s\n", index, opCodeAsString);

        logger(line);

        return 1;
    }
    else if (opCode == OP_ADD)
    {
        const char *opCodeAsString = "OP_ADD";
        int length = snprintf(NULL, 0, "%04d %s\n", index, opCodeAsString);

        char *line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%04d %s\n", index, opCodeAsString);

        logger(line);

        return 1;
    }
    else if (opCode == OP_MULT)
    {
        const char *opCodeAsString = "OP_MULT";
        int length = snprintf(NULL, 0, "%04d %s\n", index, opCodeAsString);

        char *line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%04d %s\n", index, opCodeAsString);

        logger(line);

        return 1;
    }
    else if (opCode == OP_NEGATE)
    {
        const char *opCodeAsString = "OP_NEGATE";
        int length = snprintf(NULL, 0, "%04d %s\n", index, opCodeAsString);

        char *line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%04d %s\n", index, opCodeAsString);

        logger(line);

        return 1;
    }
    else if (opCode == OP_SUB)
    {
        const char *opCodeAsString = "OP_SUB";
        int length = snprintf(NULL, 0, "%04d %s\n", index, opCodeAsString);

        char *line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%04d %s\n", index, opCodeAsString);

        logger(line);

        return 1;
    }
    else if (opCode == OP_DIV)
    {
        const char *opCodeAsString = "OP_DIV";
        int length = snprintf(NULL, 0, "%04d %s\n", index, opCodeAsString);

        char *line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%04d %s\n", index, opCodeAsString);

        logger(line);

        return 1;
    }
    else if (opCode == OP_TRUE)
    {
        const char *opCodeAsString = "OP_TRUE";
        int length = snprintf(NULL, 0, "%04d %s\n", index, opCodeAsString);

        char *line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%04d %s\n", index, opCodeAsString);
        logger(line);

        return 1;
    }
    else if (opCode == OP_FALSE)
    {
        const char *opCodeAsString = "OP_FALSE";
        int length = snprintf(NULL, 0, "%04d %s\n", index, opCodeAsString);

        char *line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%04d %s\n", index, opCodeAsString);
        logger(line);

        return 1;
    }
    else if (opCode == OP_EQUAL)
    {
        const char *opCodeAsString = "OP_EQUAL";
        int length = snprintf(NULL, 0, "%04d %s\n", index, opCodeAsString);

        char *line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%04d %s\n", index, opCodeAsString);
        logger(line);

        return 1;
    }
    else if (opCode == OP_VAR_DECL)
    {
        const char *opCodeAsString = "OP_VAR_DECL";
        int length = snprintf(NULL, 0, "%04d %s\n", index, opCodeAsString);

        char *line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%04d %s\n", index, opCodeAsString);
        logger(line);

        return 1;
    }
    else if (opCode == OP_VAR_ASSIGN)
    {
        const char *opCodeAsString = "OP_VAR_ASSIGN";
        int offset = bytecode->code[index + 1];
        int length = snprintf(NULL, 0, "%04d %s %d\n", index, opCodeAsString, offset);

        char *line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%04d %s %d\n", index, opCodeAsString, offset);
        logger(line);

        return 2;
    }
    else if (opCode == OP_VAR_EXPRESSION)
    {
        const char *opCodeAsString = "OP_VAR_EXPRESSION";
        int offset = bytecode->code[index + 1];
        int length = snprintf(NULL, 0, "%04d %s %d\n", index, opCodeAsString, offset);

        char *line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%04d %s %d\n", index, opCodeAsString, offset);
        logger(line);

        return 2;
    }
    else if (opCode == OP_POP)
    {
        const char *opCodeAsString = "OP_POP";
        int length = snprintf(NULL, 0, "%04d %s\n", index, opCodeAsString);

        char *line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%04d %s\n", index, opCodeAsString);
        logger(line);

        return 1;
    }
    else if (opCode == OP_JUMP_IF_FALSE)
    {
        const char *opCodeAsString = "OP_JUMP_IF_FALSE";
        uint16_t jumpLocation = readShort(bytecode, index + 1);
        int length = snprintf(NULL, 0, "%04d %s %d\n", index, opCodeAsString, jumpLocation);

        char *line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%04d %s %d\n", index, opCodeAsString, jumpLocation);
        logger(line);

        return 3;
    }
    else if (opCode == OP_JUMP)
    {
        const char *opCodeAsString = "OP_JUMP";
        uint16_t jumpLocation = readShort(bytecode, index + 1);
        int length = snprintf(NULL, 0, "%04d %s %d\n", index, opCodeAsString, jumpLocation);

        char *line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%04d %s %d\n", index, opCodeAsString, jumpLocation);
        logger(line);

        return 3;
    }
    else if (opCode == OP_LOOP)
    {
        const char *opCodeAsString = "OP_LOOP";
        int offset = bytecode->code[index + 1];
        int length = snprintf(NULL, 0, "%04d %s %d\n", index, opCodeAsString, offset);

        char *line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%04d %s %d\n", index, opCodeAsString, offset);
        logger(line);

        return 2;
    }
    else if (opCode == OP_LESS_THAN)
    {
        const char *opCodeAsString = "OP_LESS_THAN";
        int length = snprintf(NULL, 0, "%04d %s\n", index, opCodeAsString);

        char *line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%04d %s\n", index, opCodeAsString);
        logger(line);

        return 1;
    }
    else if (opCode == OP_LESS_THAN_EQUALS)
    {
        const char *opCodeAsString = "OP_LESS_THAN_EQUALS";
        int length = snprintf(NULL, 0, "%04d %s\n", index, opCodeAsString);

        char *line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%04d %s\n", index, opCodeAsString);
        logger(line);

        return 1;
    }
    else if (opCode == OP_CALL)
    {
        const char *opCodeAsString = "OP_CALL";
        uint8_t numArguments = bytecode->code[index + 1];
        int length = snprintf(NULL, 0, "%04d %s %d\n", index, opCodeAsString, numArguments);

        char *line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%04d %s %d\n", index, opCodeAsString, numArguments);
        logger(line);

        return 2;
    }
    else if (opCode == OP_OR)
    {
        const char *opCodeAsString = "OP_OR";
        int length = snprintf(NULL, 0, "%04d %s\n", index, opCodeAsString);

        char *line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%04d %s\n", index, opCodeAsString);
        logger(line);

        return 1;
    }
    else
    {
        const char *opCodeAsString = "BAD_OP_CODE";
        int length = snprintf(NULL, 0, "%04d %s %i\n", index, opCodeAsString, opCode);
        char *line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%04d %s %i\n", index, opCodeAsString, opCode);

        logger(line);
        return 1;
    }
}