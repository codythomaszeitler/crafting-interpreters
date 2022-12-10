#include "unity.h"
#include "compiler.h"
#include "stdint.h"
#include "chunk.h"
#include "disassembler.h"
#include "vm.h"

Interpreter testObject;

static char *test_messages[100];
uint32_t test_messages_size = 0;

static void logWhenDisassemble(char *message)
{
    test_messages[test_messages_size] = message;
    test_messages_size++;
}

void setUp()
{
    initInterpreter(&testObject);
    test_messages_size = 0;
}

void tearDown()
{
    freeInterpreter(&testObject);
}

void testItShouldBeAbleToCompileExpression()
{
    const char *sourceCode = "5";
    runInterpreter(&testObject, sourceCode);
    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("5.0", test_messages[0]);
}

void testItShouldCompileSingleNumberExpression()
{
    const char *sourceCode = "5";

    Chunk bytecode;
    initChunk(&bytecode);
    compile(&bytecode, sourceCode);

    disassembleChunk(&bytecode, "test chunk", logWhenDisassemble);

    TEST_ASSERT_EQUAL(2, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_CONSTANT 0 5.000000\n", test_messages[1]);
}

void testItShouldParseAddition()
{
    const char *sourceCode = "5 + 9";

    Chunk bytecode;
    initChunk(&bytecode);
    compile(&bytecode, sourceCode);

    disassembleChunk(&bytecode, "test chunk", logWhenDisassemble);

    TEST_ASSERT_EQUAL(4, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_CONSTANT 0 5.000000\n", test_messages[1]);
    TEST_ASSERT_EQUAL_STRING("0002 OP_CONSTANT 1 9.000000\n", test_messages[2]);
    TEST_ASSERT_EQUAL_STRING("0004 OP_ADD\n", test_messages[3]);
}

void testItShouldParseMultiplication()
{
    const char *sourceCode = "5 * 9";

    Chunk bytecode;
    initChunk(&bytecode);
    compile(&bytecode, sourceCode);

    disassembleChunk(&bytecode, "test chunk", logWhenDisassemble);

    TEST_ASSERT_EQUAL(4, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_CONSTANT 0 5.000000\n", test_messages[1]);
    TEST_ASSERT_EQUAL_STRING("0002 OP_CONSTANT 1 9.000000\n", test_messages[2]);
    TEST_ASSERT_EQUAL_STRING("0004 OP_MULT\n", test_messages[3]);
}

void testItShouldParseAdditionAndMultiplication()
{
    const char *sourceCode = "3 + 5 * 9";
    // 3 + ((5 * 9) * 10)

    Chunk bytecode;
    initChunk(&bytecode);
    compile(&bytecode, sourceCode);

    disassembleChunk(&bytecode, "test chunk", logWhenDisassemble);

    TEST_ASSERT_EQUAL(6, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_CONSTANT 0 3.000000\n", test_messages[1]);
    TEST_ASSERT_EQUAL_STRING("0002 OP_CONSTANT 1 5.000000\n", test_messages[2]);
    TEST_ASSERT_EQUAL_STRING("0004 OP_CONSTANT 2 9.000000\n", test_messages[3]);
    TEST_ASSERT_EQUAL_STRING("0006 OP_MULT\n", test_messages[4]);
    TEST_ASSERT_EQUAL_STRING("0007 OP_ADD\n", test_messages[5]);
}

void testItShouldParseMultiplicationExpressions()
{
    const char *sourceCode = "3 + 5 * 9 * 8";

    Chunk bytecode;
    initChunk(&bytecode);
    compile(&bytecode, sourceCode);

    disassembleChunk(&bytecode, "test chunk", logWhenDisassemble);

    TEST_ASSERT_EQUAL(8, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_CONSTANT 0 3.000000\n", test_messages[1]);
    TEST_ASSERT_EQUAL_STRING("0002 OP_CONSTANT 1 5.000000\n", test_messages[2]);
    TEST_ASSERT_EQUAL_STRING("0004 OP_CONSTANT 2 9.000000\n", test_messages[3]);
    TEST_ASSERT_EQUAL_STRING("0006 OP_MULT\n", test_messages[4]);
    TEST_ASSERT_EQUAL_STRING("0007 OP_CONSTANT 3 8.000000\n", test_messages[5]);
    TEST_ASSERT_EQUAL_STRING("0009 OP_MULT\n", test_messages[6]);
    TEST_ASSERT_EQUAL_STRING("0010 OP_ADD\n", test_messages[7]);
}

void testItShouldParseNegationOfNumber()
{
    const char *sourceCode = "-3";

    Chunk bytecode;
    initChunk(&bytecode);
    compile(&bytecode, sourceCode);

    disassembleChunk(&bytecode, "test chunk", logWhenDisassemble);

    TEST_ASSERT_EQUAL(3, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_CONSTANT 0 3.000000\n", test_messages[1]);
    TEST_ASSERT_EQUAL_STRING("0002 OP_NEGATE\n", test_messages[2]);
}

void testItShouldBeAbleParseNegationOnBothSidesOfMultiplication()
{
    const char *sourceCode = "-3 * -5";

    Chunk bytecode;
    initChunk(&bytecode);
    compile(&bytecode, sourceCode);

    disassembleChunk(&bytecode, "test chunk", logWhenDisassemble);

    TEST_ASSERT_EQUAL(6, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_CONSTANT 0 3.000000\n", test_messages[1]);
    TEST_ASSERT_EQUAL_STRING("0002 OP_NEGATE\n", test_messages[2]);
    TEST_ASSERT_EQUAL_STRING("0003 OP_CONSTANT 1 5.000000\n", test_messages[3]);
    TEST_ASSERT_EQUAL_STRING("0005 OP_NEGATE\n", test_messages[4]);
    TEST_ASSERT_EQUAL_STRING("0006 OP_MULT\n", test_messages[5]);
}

void testItShouldParseBasicSubtraction()
{
    const char *sourceCode = "3 - 5";

    Chunk bytecode;
    initChunk(&bytecode);
    compile(&bytecode, sourceCode);

    disassembleChunk(&bytecode, "test chunk", logWhenDisassemble);

    TEST_ASSERT_EQUAL(4, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_CONSTANT 0 3.000000\n", test_messages[1]);
    TEST_ASSERT_EQUAL_STRING("0002 OP_CONSTANT 1 5.000000\n", test_messages[2]);
    TEST_ASSERT_EQUAL_STRING("0004 OP_SUB\n", test_messages[3]);
}

void testItShouldParseDivision()
{
    const char *sourceCode = "3 / 5";

    Chunk bytecode;
    initChunk(&bytecode);
    compile(&bytecode, sourceCode);

    disassembleChunk(&bytecode, "test chunk", logWhenDisassemble);
    TEST_ASSERT_EQUAL(4, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_CONSTANT 0 3.000000\n", test_messages[1]);
    TEST_ASSERT_EQUAL_STRING("0002 OP_CONSTANT 1 5.000000\n", test_messages[2]);
    TEST_ASSERT_EQUAL_STRING("0004 OP_DIV\n", test_messages[3]);

    freeChunk(&bytecode);
}

void testItShouldRunBasicAddition()
{
    const char *sourceCode = "3 + 5";

    Interpreter testObject;
    initInterpreter(&testObject);

    runInterpreter(&testObject, sourceCode);

    Value calculated = peek(&testObject.vm);
    TEST_ASSERT_EQUAL(8, calculated);
}

void testItShouldDoComplexAdditionAndMultiplication()
{
    const char *sourceCode = "5 + 3 * 5 + 4 + 5 * 8";

    Interpreter testObject;
    initInterpreter(&testObject);

    runInterpreter(&testObject, sourceCode);

    Value calculated = peek(&testObject.vm);
    TEST_ASSERT_EQUAL(64, calculated);
}

void testItShouldDoComplexAdditionAndMultiplicationAndDivision()
{
    const char *sourceCode = "5 + 3 * 5 + 4 + 5 * 8 / 4";

    Interpreter testObject;
    initInterpreter(&testObject);

    runInterpreter(&testObject, sourceCode);

    Value calculated = peek(&testObject.vm);
    TEST_ASSERT_EQUAL(34, calculated);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(testItShouldCompileSingleNumberExpression);
    RUN_TEST(testItShouldParseAddition);
    RUN_TEST(testItShouldParseMultiplication);
    RUN_TEST(testItShouldParseAdditionAndMultiplication);
    RUN_TEST(testItShouldParseMultiplicationExpressions);
    RUN_TEST(testItShouldParseNegationOfNumber);
    RUN_TEST(testItShouldBeAbleParseNegationOnBothSidesOfMultiplication);
    RUN_TEST(testItShouldParseBasicSubtraction);
    RUN_TEST(testItShouldParseDivision);
    RUN_TEST(testItShouldRunBasicAddition);
    RUN_TEST(testItShouldDoComplexAdditionAndMultiplication);
    RUN_TEST(testItShouldDoComplexAdditionAndMultiplicationAndDivision);
    return UNITY_END();
}