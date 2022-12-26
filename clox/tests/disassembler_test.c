#include "unity.h"
#include "chunk.h"
#include "stdint.h"
#include "disassembler.h"
#include <stdlib.h>

char *disassembler_test_messages[100];
int disassembler_test_size;
void logWhenDisassemble(char *message)
{
    disassembler_test_messages[disassembler_test_size] = message;
    disassembler_test_size = disassembler_test_size + 1;
}

Chunk bytecode;
void setUp()
{
    initChunk(&bytecode);
    disassembler_test_size = 0;
}

// I wonder if there is an invariant that we could always check here...
void tearDown()
{
    freeChunk(&bytecode);
    for (int i = 0; i < disassembler_test_size; i++)
    {
        char *toFree = disassembler_test_messages[i];
        free(toFree);
        disassembler_test_messages[i] = NULL;
    }
}

void testItShouldBeAbleToCallbackToFunction()
{
    void (*toCall)(char *message) = logWhenDisassemble;

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

void testItShouldPrintReturnStatement()
{
    void (*toCall)(char *message) = logWhenDisassemble;
    writeChunk(&bytecode, OP_RETURN);

    disassembleChunk(&bytecode, "test chunk", toCall);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", disassembler_test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_RETURN\n", disassembler_test_messages[1]);
}

void testItShouldDisassemblePrintOpCode()
{
    writeChunk(&bytecode, OP_PRINT);

    disassembleChunk(&bytecode, "test chunk", logWhenDisassemble);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", disassembler_test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_PRINT\n", disassembler_test_messages[1]);
}

void testItShouldChewThroughNonOpCodes()
{
    void (*toCall)(char *message) = logWhenDisassemble;
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

void testItShouldDisassembleTrueOpCode()
{
    writeChunk(&bytecode, OP_TRUE);
    disassembleChunk(&bytecode, "test chunk", logWhenDisassemble);
    TEST_ASSERT_EQUAL(2, disassembler_test_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", disassembler_test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_TRUE\n", disassembler_test_messages[1]);
}

void testItShouldDisassembleFalseOpCode()
{
    writeChunk(&bytecode, OP_FALSE);
    disassembleChunk(&bytecode, "test chunk", logWhenDisassemble);
    TEST_ASSERT_EQUAL(2, disassembler_test_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", disassembler_test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_FALSE\n", disassembler_test_messages[1]);
}

void testItShouldDisassembleEqualsOpCode()
{
    writeChunk(&bytecode, OP_EQUAL);
    disassembleChunk(&bytecode, "test chunk", logWhenDisassemble);
    TEST_ASSERT_EQUAL(2, disassembler_test_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", disassembler_test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_EQUAL\n", disassembler_test_messages[1]);
}

void testItShouldDisassembleOpVarDeclaration()
{
    writeChunk(&bytecode, OP_VAR_DECL);
    disassembleChunk(&bytecode, "test chunk", logWhenDisassemble);
    TEST_ASSERT_EQUAL(2, disassembler_test_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", disassembler_test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_VAR_DECL\n", disassembler_test_messages[1]);
}

void testItShouldDisassembleOpVarAssign()
{
    uint8_t stackOffset = 5;
    writeChunk(&bytecode, OP_VAR_ASSIGN);
    writeChunk(&bytecode, stackOffset);
    disassembleChunk(&bytecode, "test chunk", logWhenDisassemble);
    TEST_ASSERT_EQUAL(2, disassembler_test_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", disassembler_test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_VAR_ASSIGN 5\n", disassembler_test_messages[1]);
}

void testItShouldDisassembleOpVarExpression()
{
    uint8_t stackOffset = 1;
    writeChunk(&bytecode, OP_VAR_EXPRESSION);
    writeChunk(&bytecode, stackOffset);
    disassembleChunk(&bytecode, "test chunk", logWhenDisassemble);
    TEST_ASSERT_EQUAL(2, disassembler_test_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", disassembler_test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_VAR_EXPRESSION 1\n", disassembler_test_messages[1]);
}

void testItShouldUseOpPopExpression()
{
    writeChunk(&bytecode, OP_POP);
    disassembleChunk(&bytecode, "test chunk", logWhenDisassemble);
    TEST_ASSERT_EQUAL(2, disassembler_test_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", disassembler_test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_POP\n", disassembler_test_messages[1]);
}

void testItShouldDisassembleOpJumpIfFalse()
{
    uint16_t jumpLocation = UINT16_MAX;
    writeChunk(&bytecode, OP_JUMP_IF_FALSE);
    writeShort(&bytecode, jumpLocation);

    disassembleChunk(&bytecode, "test chunk", logWhenDisassemble);
    TEST_ASSERT_EQUAL(2, disassembler_test_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", disassembler_test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_JUMP_IF_FALSE 65535\n", disassembler_test_messages[1]);
}

void testItShouldDisassembleOpJump()
{
    uint16_t jumpLocation = UINT16_MAX;
    writeChunk(&bytecode, OP_JUMP);
    writeShort(&bytecode, jumpLocation);

    disassembleChunk(&bytecode, "test chunk", logWhenDisassemble);
    TEST_ASSERT_EQUAL(2, disassembler_test_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", disassembler_test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_JUMP 65535\n", disassembler_test_messages[1]);
}

void testItShouldDisassembleOpLoop()
{
    uint8_t jumpOffset = 150;
    writeChunk(&bytecode, OP_LOOP);
    writeChunk(&bytecode, jumpOffset);
    disassembleChunk(&bytecode, "test chunk", logWhenDisassemble);
    TEST_ASSERT_EQUAL(2, disassembler_test_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", disassembler_test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_LOOP 150\n", disassembler_test_messages[1]);
}

void testItShouldDisassembleOpLessThan()
{
    writeChunk(&bytecode, OP_LESS_THAN);
    disassembleChunk(&bytecode, "test chunk", logWhenDisassemble);
    TEST_ASSERT_EQUAL(2, disassembler_test_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", disassembler_test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_LESS_THAN\n", disassembler_test_messages[1]);
}

void testItShouldDisassembleOpLessThanOrEquals()
{
    writeChunk(&bytecode, OP_LESS_THAN_EQUALS);
    disassembleChunk(&bytecode, "test chunk", logWhenDisassemble);
    TEST_ASSERT_EQUAL(2, disassembler_test_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", disassembler_test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_LESS_THAN_EQUALS\n", disassembler_test_messages[1]);
}

void testItShouldDisassembleOpCall()
{
    uint8_t functionConstantIndex = 5;
    writeChunk(&bytecode, OP_CALL);
    writeChunk(&bytecode, functionConstantIndex);
    disassembleChunk(&bytecode, "test chunk", logWhenDisassemble);
    TEST_ASSERT_EQUAL(2, disassembler_test_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", disassembler_test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_CALL 5\n", disassembler_test_messages[1]);
}

void testItShouldDisassembleOpOr() 
{
    writeChunk(&bytecode, OP_OR);
    disassembleChunk(&bytecode, "test chunk", logWhenDisassemble);
    TEST_ASSERT_EQUAL(2, disassembler_test_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", disassembler_test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_OR\n", disassembler_test_messages[1]);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(testItShouldBeAbleToCallbackToFunction);
    RUN_TEST(testItShouldChewThroughNonOpCodes);
    RUN_TEST(testItShouldPrintReturnStatement);
    RUN_TEST(testItShouldDisassemblePrintOpCode);
    RUN_TEST(testItShouldDisassembleTrueOpCode);
    RUN_TEST(testItShouldDisassembleFalseOpCode);
    RUN_TEST(testItShouldDisassembleEqualsOpCode);
    RUN_TEST(testItShouldDisassembleOpVarDeclaration);
    RUN_TEST(testItShouldDisassembleOpVarAssign);
    RUN_TEST(testItShouldDisassembleOpVarExpression);
    RUN_TEST(testItShouldUseOpPopExpression);
    RUN_TEST(testItShouldDisassembleOpJumpIfFalse);
    RUN_TEST(testItShouldDisassembleOpJump);
    RUN_TEST(testItShouldDisassembleOpLoop);
    RUN_TEST(testItShouldDisassembleOpLessThan);
    RUN_TEST(testItShouldDisassembleOpLessThanOrEquals);
    RUN_TEST(testItShouldDisassembleOpCall);
    RUN_TEST(testItShouldDisassembleOpOr);
    return UNITY_END();
}