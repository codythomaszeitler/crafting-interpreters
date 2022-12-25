#include "unity.h"
#include "compiler.h"
#include "chunk.h"
#include "disassembler.h"
#include <stdbool.h>
#include "value.h"
#include "cloxstring.h"
#include "functionobj.h"

static char *test_messages[100];
uint32_t test_messages_size = 0;

static void logWhenDisassemble(char *message)
{
    test_messages[test_messages_size] = message;
    test_messages_size++;
}

static void disassembleTest(const char *sourceCode)
{
    FunctionObj function;
    initFunctionObj(&function);

    TokenArrayIterator tokens = tokenize(sourceCode);
    compile(&function, &tokens);

    disassembleChunk(function.bytecode, "test chunk", logWhenDisassemble);
}

void setUp()
{
    test_messages_size = 0;
}

void tearDown()
{
    for (int i = 0; i < test_messages_size; i++)
    {
        free(test_messages[i]);
    }
}

void testItShouldCompileSingleNumberExpression()
{
    const char *sourceCode = "5";

    disassembleTest(sourceCode);

    TEST_ASSERT_EQUAL(3, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_CONSTANT 0 5.000000\n", test_messages[1]);
}

void testItShouldParseAddition()
{
    const char *sourceCode = "5 + 9";

    disassembleTest(sourceCode);

    TEST_ASSERT_EQUAL(5, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_CONSTANT 0 5.000000\n", test_messages[1]);
    TEST_ASSERT_EQUAL_STRING("0002 OP_CONSTANT 1 9.000000\n", test_messages[2]);
    TEST_ASSERT_EQUAL_STRING("0004 OP_ADD\n", test_messages[3]);
}

void testItShouldParseMultiplication()
{
    const char *sourceCode = "5 * 9";

    disassembleTest(sourceCode);

    TEST_ASSERT_EQUAL(5, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_CONSTANT 0 5.000000\n", test_messages[1]);
    TEST_ASSERT_EQUAL_STRING("0002 OP_CONSTANT 1 9.000000\n", test_messages[2]);
    TEST_ASSERT_EQUAL_STRING("0004 OP_MULT\n", test_messages[3]);
}

void testItShouldParseAdditionAndMultiplication()
{
    const char *sourceCode = "3 + 5 * 9";
    // 3 + ((5 * 9) * 10)

    disassembleTest(sourceCode);

    TEST_ASSERT_EQUAL(7, test_messages_size);
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

    disassembleTest(sourceCode);

    TEST_ASSERT_EQUAL(9, test_messages_size);
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

    disassembleTest(sourceCode);

    TEST_ASSERT_EQUAL(4, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_CONSTANT 0 3.000000\n", test_messages[1]);
    TEST_ASSERT_EQUAL_STRING("0002 OP_NEGATE\n", test_messages[2]);
}

void testItShouldBeAbleParseNegationOnBothSidesOfMultiplication()
{
    const char *sourceCode = "-3 * -5";

    disassembleTest(sourceCode);

    TEST_ASSERT_EQUAL(7, test_messages_size);
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

    disassembleTest(sourceCode);

    TEST_ASSERT_EQUAL(5, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_CONSTANT 0 3.000000\n", test_messages[1]);
    TEST_ASSERT_EQUAL_STRING("0002 OP_CONSTANT 1 5.000000\n", test_messages[2]);
    TEST_ASSERT_EQUAL_STRING("0004 OP_SUB\n", test_messages[3]);
}

void testItShouldParseDivision()
{
    const char *sourceCode = "3 / 5";

    disassembleTest(sourceCode);

    TEST_ASSERT_EQUAL(5, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("=== test chunk ===\n", test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("0000 OP_CONSTANT 0 3.000000\n", test_messages[1]);
    TEST_ASSERT_EQUAL_STRING("0002 OP_CONSTANT 1 5.000000\n", test_messages[2]);
    TEST_ASSERT_EQUAL_STRING("0004 OP_DIV\n", test_messages[3]);
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
    return UNITY_END();
}