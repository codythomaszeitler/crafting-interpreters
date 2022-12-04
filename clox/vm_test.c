#include "unity.h"
#include "chunk.h"
#include "vm.h"

VirtualMachine testObject;
void setUp() {
    initVirtualMachine(&testObject);
}

void tearDown() {}

void testItShouldBeAbleToAddConstantToVMStack() {
    Chunk bytecode; 
    initChunk(&bytecode);

    int indexConstant = addConstant(&bytecode, 5.0);
    writeChunk(&bytecode, OP_CONSTANT);
    writeChunk(&bytecode, indexConstant);

    writeChunk(&bytecode, OP_NEGATE);
    writeChunk(&bytecode, OP_CONSTANT);
    writeChunk(&bytecode, indexConstant);

    writeChunk(&bytecode, OP_ADD);
    interpret(&testObject, &bytecode);
    Value value = peek(&testObject);
    TEST_ASSERT_EQUAL(0, value);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testItShouldBeAbleToAddConstantToVMStack);
    return UNITY_END();
}