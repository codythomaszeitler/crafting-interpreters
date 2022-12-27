#include "unity.h"
#include "unity.h"
#include "compiler.h"
#include "chunk.h"
#include "disassembler.h"
#include "vm.h"
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

Interpreter testObject;
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

void testItShouldBeAbleToDoMultiLevelFunctionCalls()
{
    const char *sourceCode = "{ func foo(a) { func bar(b) { print b;} bar(a); } foo(5); }";

    runInterpreter(&testObject, sourceCode);
    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("5.000000", test_messages[0]);
}

void testItShouldDoMultiArgFunctionCalls()
{
    const char *sourceCode = "{ func foo(a, b, c) { print a + b + c;} foo(1, 2, 3); }";
    runInterpreter(&testObject, sourceCode);
    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("6.000000", test_messages[0]);
}

void testItShouldBeAbleToDoFunctionCallsWithinExpression()
{
    const char *sourceCode = "{ func foo(a) {return a + 1;} var c = foo(1) + foo(2) + foo(3); print c; }";
    runInterpreter(&testObject, sourceCode);
    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("9.000000", test_messages[0]);
}

void testItShouldHaveTwoReturnsInFunction()
{
    const char *sourceCode = "{func foo(a) { if (a <= 1) {return a;} return a + 1;} var c = foo(2); print c;}";
    runInterpreter(&testObject, sourceCode);
    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("3.000000", test_messages[0]);
}

void testItShouldGoToBaseCase()
{
    const char *sourceCode = "{func foo(a) { if (a <= 1) {return a;} return a + 1;} var c = foo(1); print c;}";
    runInterpreter(&testObject, sourceCode);
    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("1.000000", test_messages[0]);
}

void testItShouldBeAbleToDoOrStatement()
{
    const char *sourceCode = "{ var a = true || false; print a;}";
    runInterpreter(&testObject, sourceCode);
    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("true", test_messages[0]);
}

void testItShouldBeAbleToDoLessThanOrEqualsTo()
{
    const char *sourceCode = "{ var a = 2 <= 5; print a;}";
    runInterpreter(&testObject, sourceCode);
    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("true", test_messages[0]);
}

void testItShouldBeAbleToDoLessThanOrEqualsToSameNum()
{
    const char *sourceCode = "{ var a = 5 <= 5; print a;}";
    runInterpreter(&testObject, sourceCode);
    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("true", test_messages[0]);
}

void testItShouldBeAbleToDoLessThanOrEqualsToWhereLeftIsGreater()
{
    const char *sourceCode = "{ var a = 6 <= 5; print a;}";
    runInterpreter(&testObject, sourceCode);
    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("false", test_messages[0]);
}

void testItShouldBeAbleToDoInlineExpressionInFuncCall()
{
    const char *sourceCode = "{func foo(n) { return n;} print foo(5 - 3);}";
    runInterpreter(&testObject, sourceCode);
    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("2.000000", test_messages[0]);
}

void testItShouldDoSimpleRecursionBaseCase()
{
    const char *sourceCode = "{func foo(n) { if (n <= 1) {return 1;} return n - 1; } print foo(1);}";
    runInterpreter(&testObject, sourceCode);
    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("1.000000", test_messages[0]);
}

void testItShouldDoSimpleRecursion()
{
    const char *sourceCode = "{func foo(n) { if (n <= 1) {return 1;} return foo(n - 1); } print foo(2);}";
    runInterpreter(&testObject, sourceCode);
    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("1.000000", test_messages[0]);
}

void testItShouldDoFibNumbers()
{
    const char *sourceCode = "{func fib(n) { if (n <= 1) {return n;} return fib (n - 2) + fib(n - 1);} print fib(9);}";
    runInterpreter(&testObject, sourceCode);
    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("34.000000", test_messages[0]);
}

void testItShouldBeAbleToReturnACallAFunction()
{
    const char *sourceCode = "{ func foo() { func bar() {print 1; } return bar; } var a = foo(); a();}";
    runInterpreter(&testObject, sourceCode);
    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("1.000000", test_messages[0]);
}

void testItShouldBeAbleToCloseAroundImmediatelyEnclosingFunction()
{
    // So there are two new opcodes that are going to be introduced here.
    // OP_GET_UPVALUE
    // OP_SET_UPVALUE
    // OP_CLOSURE has already been 
    // So once you are resolving the identifier expression, you must have to go up the compiler function stack
    // until you find a variable matching a
    // So remember, a is exactly equivalent to a slot 
    // But we have no idea where to find a? 
    // Wait wait wait... how do we know if the 
    // There is a problem, getVariableBinding works through the current compiler. 

    // Wherever getVariableBinding is used.. we really need to change that to be called with current compiler?
    // variableDecl, and identifier is where this is used.
    // We really need to change it from using a parser to using a current compiler? 
    const char *sourceCode = "{ func foo() { var a = 1; func bar() {print a; } return bar; } var z = foo(); z();}";
    runInterpreter(&testObject, sourceCode);
    TEST_ASSERT_EQUAL(1, test_messages_size);
    TEST_ASSERT_EQUAL_STRING("1.000000", test_messages[0]);
}

int main(void)
{
    UNITY_BEGIN();
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
    RUN_TEST(testItShouldBeAbleToDoMultiLevelFunctionCalls);
    RUN_TEST(testItShouldDoMultiArgFunctionCalls);
    RUN_TEST(testItShouldBeAbleToDoFunctionCallsWithinExpression);
    RUN_TEST(testItShouldHaveTwoReturnsInFunction);
    RUN_TEST(testItShouldGoToBaseCase);
    RUN_TEST(testItShouldBeAbleToDoOrStatement);
    RUN_TEST(testItShouldBeAbleToDoLessThanOrEqualsTo);
    RUN_TEST(testItShouldBeAbleToDoLessThanOrEqualsToSameNum);
    RUN_TEST(testItShouldBeAbleToDoLessThanOrEqualsToWhereLeftIsGreater);
    RUN_TEST(testItShouldBeAbleToDoInlineExpressionInFuncCall);
    RUN_TEST(testItShouldDoSimpleRecursionBaseCase);
    RUN_TEST(testItShouldDoSimpleRecursion);
    RUN_TEST(testItShouldDoFibNumbers);
    RUN_TEST(testItShouldBeAbleToReturnACallAFunction);
    // RUN_TEST(testItShouldBeAbleToCloseAroundImmediatelyEnclosingFunction);
    return UNITY_END();
}