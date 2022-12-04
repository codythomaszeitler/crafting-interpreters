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

void setUp() {
    disassembler_test_size = 0;
}

void tearDown() {
    for (int i = 0; i < disassembler_test_size; i++) {
        free(disassembler_test_messages[i]);
        disassembler_test_messages[i] = NULL;
    }
}

void testItShouldBeAbleToCallbackToFunction() {
    Chunk bytecode;
    initChunk(&bytecode);

    void (*toCall) (char* message) = logWhenDisassemble;

    int constantIndex = addConstant(&bytecode, 5.0);

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

void testItShouldChewThroughNonOpCodes() {
    Chunk testObject;
    initChunk(&testObject);

    void (*toCall) (char* message) = logWhenDisassemble;
    writeChunk(&testObject, -1);
    writeChunk(&testObject, -2);
    writeChunk(&testObject, -3);
    writeChunk(&testObject, OP_RETURN);

    disassembleChunk(&testObject, "test chunk", toCall);

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
    return UNITY_END();
}