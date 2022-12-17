#include "unity.h"
#include "chunk.h"
#include "stdint.h"
#include "disassembler.h"
#include <stdlib.h>

char* disassembler_test_messages[100];
int disassembler_test_size;
void logWhenDisassemble(char* message) {
    disassembler_test_messages[disassembler_test_size] = message;
    disassembler_test_size = disassembler_test_size + 1;
}


Chunk bytecode;
void setUp() {
    initChunk(&bytecode);
    disassembler_test_size = 0;
}

void tearDown() {
    freeChunk(&bytecode);
    for (int i = 0; i < disassembler_test_size; i++) {
        char* toFree = disassembler_test_messages[i];
        free(toFree);
        disassembler_test_messages[i] = NULL;
    }
}

void testItShouldBeAbleToCallbackToFunction() {
    void (*toCall) (char* message) = logWhenDisassemble;

    int constantIndex = addConstant(&bytecode, wrapNumber(5.0));

    writeChunk(&bytecode, OP_RETURN);
    writeChunk(&bytecode, OP_CONSTANT);
    writeChunk(&bytecode, constantIndex);
    writeChunk(&bytecode, OP_NEGATE);

    disassembleChunk(&bytecode, "test chunk", toCall);

    TEST_ASSERT_EQUAL(4, disassembler_test_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", disassembler_test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_RETURN\n", disassembler_test_messages[1]);
    TEST_ASSERT_EQUAL_STRING("0001 OP_CONSTANT 0 5.000000\n", disassembler_test_messages[2]);
    TEST_ASSERT_EQUAL_STRING("0003 OP_NEGATE\n", disassembler_test_messages[3]);
}

void testItShouldPrintReturnStatement() {
    void (*toCall) (char* message) = logWhenDisassemble;
    writeChunk(&bytecode, OP_RETURN);

    disassembleChunk(&bytecode, "test chunk", toCall);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", disassembler_test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_RETURN\n", disassembler_test_messages[1]);
}

void testItShouldDisassemblePrintOpCode() {
    void (*toCall) (char* message) = logWhenDisassemble;
    writeChunk(&bytecode, OP_STACK_PEEK);

    disassembleChunk(&bytecode, "test chunk", toCall);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", disassembler_test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_STACK_PRINT\n", disassembler_test_messages[1]);
}

void testItShouldChewThroughNonOpCodes() {
    void (*toCall) (char* message) = logWhenDisassemble;
    writeChunk(&bytecode, -1);
    writeChunk(&bytecode, -2);
    writeChunk(&bytecode, -3);
    writeChunk(&bytecode, OP_RETURN);

    disassembleChunk(&bytecode, "test chunk", toCall);

    TEST_ASSERT_EQUAL(5, disassembler_test_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", disassembler_test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 BAD_OP_CODE 255\n", disassembler_test_messages[1]);
    TEST_ASSERT_EQUAL_STRING("0001 BAD_OP_CODE 254\n", disassembler_test_messages[2]);
    TEST_ASSERT_EQUAL_STRING("0002 BAD_OP_CODE 253\n", disassembler_test_messages[3]);
    TEST_ASSERT_EQUAL_STRING("0003 OP_RETURN\n", disassembler_test_messages[4]);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testItShouldBeAbleToCallbackToFunction);
    RUN_TEST(testItShouldChewThroughNonOpCodes);
    RUN_TEST(testItShouldPrintReturnStatement);
    RUN_TEST(testItShouldDisassemblePrintOpCode);
    return UNITY_END();
}