#include "unity.h"
#include "compiler.h"
#include "chunk.h"
#include "disassembler.h"
#include "vm.h"
#include <stdbool.h>
#include "value.h"
#include "cloxstring.h"
#include "functionobj.h"

Interpreter testObject;

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
    initInterpreter(&testObject);
    test_messages_size = 0;
    testObject.onStdOut = logWhenDisassemble;
}

void tearDown()
{
    freeInterpreter(&testObject);
    for (int i = 0; i < test_messages_size; i++)
    {
        free(test_messages[i]);
    }
}

void testItShouldBeAbleToCompileExpression()
{
    const char *sourceCode = "print 5;";
    runInterpreter(&testObject, sourceCode);
    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("5.0", test_messages[0]);
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

void testItShouldRunBasicAddition()
{
    const char *sourceCode = "print 3 + 5;";

    runInterpreter(&testObject, sourceCode);

    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("8.000000", test_messages[0]);
}

void testItShouldDoComplexAdditionAndMultiplication()
{
    const char *sourceCode = "print 5 + 3 * 5 + 4 + 5 * 8;";

    runInterpreter(&testObject, sourceCode);

    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("64.000000", test_messages[0]);
}

void testItShouldDoComplexAdditionAndMultiplicationAndDivision()
{
    const char *sourceCode = "print 5 + 3 * 5 + 4 + 5 * 8 / 4;";
    runInterpreter(&testObject, sourceCode);

    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("34.000000", test_messages[0]);
}

void testItShouldPopTrueOntoTopOfStack()
{
    const char *sourceCode = "print true;";
    runInterpreter(&testObject, sourceCode);

    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("true", test_messages[0]);
}

void testItShouldPopFalseOntoTopOfStack()
{
    const char *sourceCode = "print false;";
    runInterpreter(&testObject, sourceCode);

    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("false", test_messages[0]);
}

void testItShouldDoEqualityBetweenTrueAndFalse()
{
    const char *sourceCode = "print false != true;";
    runInterpreter(&testObject, sourceCode);

    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("true", test_messages[0]);
}

void testItShouldPutStringOntoStack()
{
    const char *sourceCode = "print \"cody is cool\";";
    runInterpreter(&testObject, sourceCode);

    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("cody is cool", test_messages[0]);
}

void testItShouldConcatStrings()
{
    const char *sourceCode = "print \"a\" + \"b\";";
    runInterpreter(&testObject, sourceCode);

    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("ab", test_messages[0]);
}

void testItShouldPrintSimpleExpression()
{
    const char *sourceCode = "print 5;";
    testObject.onStdOut = logWhenDisassemble;
    runInterpreter(&testObject, sourceCode);

    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("5.000000", test_messages[0]);
}

void testItShouldDoSimpleAssignmentAndPrint()
{
    const char *sourceCode = "{var a; a = 1; print a;}";
    testObject.onStdOut = logWhenDisassemble;
    runInterpreter(&testObject, sourceCode);

    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("1.000000", test_messages[0]);
}

void testItShouldDoSimpleAdditionWithAssignment()
{
    const char *sourceCode = "{var a; a = 1; var b; b = 3; print a + b;}";
    testObject.onStdOut = logWhenDisassemble;
    runInterpreter(&testObject, sourceCode);

    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("4.000000", test_messages[0]);
}

void testItShouldDoAssignmentWithDecl()
{
    const char *sourceCode = "{var a = 1; print a;}";
    testObject.onStdOut = logWhenDisassemble;
    runInterpreter(&testObject, sourceCode);

    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("1.000000", test_messages[0]);
}

void testItShouldDoComplexExpressionInDecl()
{
    const char *sourceCode = "{var a = 1 + 3 * 4; print a;}";
    testObject.onStdOut = logWhenDisassemble;
    runInterpreter(&testObject, sourceCode);

    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("13.000000", test_messages[0]);
}

void testIsShouldRunWithinABlock()
{
    const char *sourceCode = "{var a; a = 3; print a;}";
    testObject.onStdOut = logWhenDisassemble;
    runInterpreter(&testObject, sourceCode);

    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("3.000000", test_messages[0]);
}

void testItShouldRunTwoBlocksAndPrintCorrectValues()
{
    const char *sourceCode = "{var d; d = 4; {var a; a = 2 + d; print a;}{var b; b = 5; print b;}}";
    testObject.onStdOut = logWhenDisassemble;
    runInterpreter(&testObject, sourceCode);

    TEST_ASSERT_EQUAL(2, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("6.000000", test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("5.000000", test_messages[1]);
}

void testItShouldBeAbleToReadFromGlobalScope()
{
    const char *sourceCode = "var a; a = 5; print a;";
    testObject.onStdOut = logWhenDisassemble;
    runInterpreter(&testObject, sourceCode);

    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("5.000000", test_messages[0]);
}

void testItShouldBeAbleToGoIntoIfConditional()
{
    const char *sourceCode = "{var a = true; if (a) { print 5; } print 3;}";
    testObject.onStdOut = logWhenDisassemble;
    runInterpreter(&testObject, sourceCode);

    TEST_ASSERT_EQUAL(2, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("5.000000", test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("3.000000", test_messages[1]);
}

void testItShouldBeAbleToNotGoIntoIfConditional()
{
    const char *sourceCode = "{var a = false; if (a) { print 5; } print 3;}";
    testObject.onStdOut = logWhenDisassemble;
    runInterpreter(&testObject, sourceCode);

    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("3.000000", test_messages[0]);
}

void testItShouldBeAbleToGoThroughWhileLoop()
{
    const char *sourceCode = "{var a = 0; while (a < 3) {print a; a = a + 1;}}";
    testObject.onStdOut = logWhenDisassemble;
    runInterpreter(&testObject, sourceCode);

    TEST_ASSERT_EQUAL(3, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("0.000000", test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("1.000000", test_messages[1]);
    TEST_ASSERT_EQUAL_STRING("2.000000", test_messages[2]);
}

void testItShouldBeAbleToGoThroughForLoop()
{
    const char *sourceCode = "{for (var i = 0; i < 3; i = i + 1;){ print i;} }";
    testObject.onStdOut = logWhenDisassemble;
    runInterpreter(&testObject, sourceCode);

    TEST_ASSERT_EQUAL(3, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("0.000000", test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("1.000000", test_messages[1]);
    TEST_ASSERT_EQUAL_STRING("2.000000", test_messages[2]);
}

void testItShouldBeAbleToDefineAndUseFunction()
{
    const char *sourceCode = "{ func foo() {print 5;} foo(); }";
    runInterpreter(&testObject, sourceCode);

    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("5.000000", test_messages[0]);
}

void testItShouldRunWithFunctionArgumentNoReturnValue()
{
    const char *sourceCode = "{ func foo(a) {print a;} foo(5); }";
    runInterpreter(&testObject, sourceCode);

    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("5.000000", test_messages[0]);
}

void testItShouldRunWithFunctionArgumentWithReturnValue()
{
    const char *sourceCode = "{ func foo(a) {return a + 1;} var b = foo(5); print b;}";
    runInterpreter(&testObject, sourceCode);

    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("6.000000", test_messages[0]);
}

void testItShouldReturnNilWhenNoReturnGiven() 
{
    const char *sourceCode = "{ func foo(a) {print a;} var b = foo(5); print b;}";
    runInterpreter(&testObject, sourceCode);

    TEST_ASSERT_EQUAL(2, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("5.000000", test_messages[0]);
    TEST_ASSERT_EQUAL_STRING("nil", test_messages[1]);
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
    RUN_TEST(testItShouldPopTrueOntoTopOfStack);
    RUN_TEST(testItShouldPopFalseOntoTopOfStack);
    RUN_TEST(testItShouldDoEqualityBetweenTrueAndFalse);
    RUN_TEST(testItShouldPutStringOntoStack);
    RUN_TEST(testItShouldConcatStrings);
    RUN_TEST(testItShouldPrintSimpleExpression);
    RUN_TEST(testItShouldDoSimpleAssignmentAndPrint);
    RUN_TEST(testItShouldDoSimpleAdditionWithAssignment);
    RUN_TEST(testItShouldDoAssignmentWithDecl);
    RUN_TEST(testItShouldDoComplexExpressionInDecl);
    RUN_TEST(testIsShouldRunWithinABlock);
    RUN_TEST(testItShouldRunTwoBlocksAndPrintCorrectValues);
    RUN_TEST(testItShouldBeAbleToReadFromGlobalScope);
    RUN_TEST(testItShouldBeAbleToGoIntoIfConditional);
    RUN_TEST(testItShouldBeAbleToNotGoIntoIfConditional);
    RUN_TEST(testItShouldBeAbleToGoThroughWhileLoop);
    RUN_TEST(testItShouldBeAbleToGoThroughForLoop);
    RUN_TEST(testItShouldBeAbleToDefineAndUseFunction);
    RUN_TEST(testItShouldRunWithFunctionArgumentNoReturnValue);
    RUN_TEST(testItShouldRunWithFunctionArgumentWithReturnValue);
    RUN_TEST(testItShouldReturnNilWhenNoReturnGiven);
    return UNITY_END();
}