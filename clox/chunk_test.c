#include "unity.h"
#include <stdio.h>
#include "chunk.h"

Chunk testObject;
void setUp() {
    initChunk(&testObject);
    TEST_ASSERT_EQUAL(0, testObject.count);
    TEST_ASSERT_EQUAL(0, testObject.capacity);
    TEST_ASSERT_NULL(testObject.code);
}

void tearDown() {
    freeChunk(&testObject);
    TEST_ASSERT_EQUAL(0, testObject.capacity);
    TEST_ASSERT_EQUAL(0, testObject.count);
    TEST_ASSERT_EQUAL(NULL, testObject.code);
}

void testAddByteToChunk() {
    writeChunk(&testObject, 1);
    writeChunk(&testObject, 2);
    writeChunk(&testObject, 3);
    writeChunk(&testObject, 4);
    writeChunk(&testObject, 5);
    writeChunk(&testObject, 6);
    writeChunk(&testObject, 7);
    writeChunk(&testObject, 8);

    for (int i = 0; i < testObject.count; i++) {
        uint8_t value = testObject.code[i];
        TEST_ASSERT_EQUAL(i + 1, value);
    }

    TEST_ASSERT_EQUAL(8, testObject.capacity);
    TEST_ASSERT_EQUAL(8, testObject.count);

    writeChunk(&testObject, 9);
    TEST_ASSERT_EQUAL(16, testObject.capacity);
    TEST_ASSERT_EQUAL(9, testObject.count);
}

void testItShouldBeAbleToAddConstant() {
    int index = addConstant(&testObject, 5.0);
    TEST_ASSERT_EQUAL(0, index);
    TEST_ASSERT_EQUAL(5.0, getConstantAt(&testObject, index));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testAddByteToChunk);
    RUN_TEST(testItShouldBeAbleToAddConstant);
    return UNITY_END();
}