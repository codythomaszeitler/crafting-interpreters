#include "unity.h"
#include "chunk.h"
#include "stdint.h"
#include "disassembler.h"
#include <stdlib.h>

char* disassembler_test_messages[100];
int disassembler_test_size;
void testLogger(char* message) {
    disassembler_test_messages[disassembler_test_size] = message;
    disassembler_test_size = disassembler_test_size + 1;
}

void setUp() {
    disassembler_test_size = 0;
}

void tearDown() {
    for (int i = 0; i < disassembler_test_size; i++) {
        free(disassembler_test_messages[i]);
    }
}

void testItShouldBeAbleToCallbackToFunction() {
    Chunk bytecode;
    initChunk(&bytecode);

    void (*toCall) (char* message) = testLogger;

    int constantIndex = addConstant(&bytecode, 5.0);

    writeChunk(&bytecode, OP_RETURN);
    writeChunk(&bytecode, OP_CONSTANT);
    writeChunk(&bytecode, constantIndex);

    disassembleChunk(&bytecode, "test chunk", toCall);

    TEST_ASSERT_EQUAL(3, disassembler_test_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", disassembler_test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_RETURN\n", disassembler_test_messages[1]);
    TEST_ASSERT_EQUAL_STRING("0001 OP_CONSTANT 0 5.000000\n", disassembler_test_messages[2]);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testItShouldBeAbleToCallbackToFunction);
    return UNITY_END();
}