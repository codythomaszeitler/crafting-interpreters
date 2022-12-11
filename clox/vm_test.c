#include "unity.h"
#include "chunk.h"
#include "vm.h"

VirtualMachine testObject;
void setUp()
{
    initVirtualMachine(&testObject);
}

void tearDown() {}

void testItShouldBeAbleToAddConstantToVMStack()
{
    Chunk bytecode;
    initChunk(&bytecode);

    int indexConstant = addConstant(&bytecode, wrapNumber(5.0));
    writeChunk(&bytecode, OP_CONSTANT);
    writeChunk(&bytecode, indexConstant);

    writeChunk(&bytecode, OP_NEGATE);
    writeChunk(&bytecode, OP_CONSTANT);
    writeChunk(&bytecode, indexConstant);

    writeChunk(&bytecode, OP_ADD);
    interpret(&testObject, &bytecode);
    Value value = peek(&testObject);
    TEST_ASSERT_EQUAL(0, unwrapNumber(value));

    freeChunk(&bytecode);
}

void testItShouldBeAbleToDoMultiplication()
{
    Chunk bytecode;
    initChunk(&bytecode);

    int indexConstant = addConstant(&bytecode, wrapNumber(5.0));
    writeChunk(&bytecode, OP_CONSTANT);
    writeChunk(&bytecode, indexConstant);

    writeChunk(&bytecode, OP_CONSTANT);
    writeChunk(&bytecode, indexConstant);

    writeChunk(&bytecode, OP_MULT);
    interpret(&testObject, &bytecode);
    Value value = peek(&testObject);
    TEST_ASSERT_EQUAL(25.0, unwrapNumber(value));

    freeChunk(&bytecode);
}

void testItShouldBeAbleToDoDivision()
{
    Chunk bytecode;
    initChunk(&bytecode);

    int twentyFiveIndex = addConstant(&bytecode, wrapNumber(25.0));
    int fiveIndex = addConstant(&bytecode, wrapNumber(5.0));
    writeChunk(&bytecode, OP_CONSTANT);
    writeChunk(&bytecode, twentyFiveIndex);

    writeChunk(&bytecode, OP_CONSTANT);
    writeChunk(&bytecode, fiveIndex);

    writeChunk(&bytecode, OP_DIV);
    interpret(&testObject, &bytecode);
    Value value = peek(&testObject);
    TEST_ASSERT_EQUAL(5.0, unwrapNumber(value));

    freeChunk(&bytecode);
}

void testItShouldBeAbleToDoSubtraction()
{
    Chunk bytecode;
    initChunk(&bytecode);

    int twentyFiveIndex = addConstant(&bytecode, wrapNumber(25.0));
    int fiveIndex = addConstant(&bytecode, wrapNumber(5.0));
    writeChunk(&bytecode, OP_CONSTANT);
    writeChunk(&bytecode, twentyFiveIndex);

    writeChunk(&bytecode, OP_CONSTANT);
    writeChunk(&bytecode, fiveIndex);

    writeChunk(&bytecode, OP_SUB);
    interpret(&testObject, &bytecode);
    Value value = peek(&testObject);
    TEST_ASSERT_EQUAL(20.0, unwrapNumber(value));

    freeChunk(&bytecode);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(testItShouldBeAbleToAddConstantToVMStack);
    RUN_TEST(testItShouldBeAbleToDoMultiplication);
    RUN_TEST(testItShouldBeAbleToDoDivision);
    RUN_TEST(testItShouldBeAbleToDoSubtraction);
    return UNITY_END();
}