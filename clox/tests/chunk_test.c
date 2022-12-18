#include "unity.h"
#include <stdio.h>
#include "chunk.h"
#include "value.h"

Chunk testObject;
void setUp()
{
    initChunk(&testObject);
    TEST_ASSERT_EQUAL(0, testObject.count);
    TEST_ASSERT_EQUAL(0, testObject.capacity);
    TEST_ASSERT_NULL(testObject.code);
}

void tearDown()
{
    freeChunk(&testObject);
    TEST_ASSERT_EQUAL(0, testObject.capacity);
    TEST_ASSERT_EQUAL(0, testObject.count);
    TEST_ASSERT_EQUAL(NULL, testObject.code);
}

void testAddByteToChunk()
{
    writeChunk(&testObject, 1);
    writeChunk(&testObject, 2);
    writeChunk(&testObject, 3);
    writeChunk(&testObject, 4);
    writeChunk(&testObject, 5);
    writeChunk(&testObject, 6);
    writeChunk(&testObject, 7);
    writeChunk(&testObject, 8);

    for (int i = 0; i < testObject.count; i++)
    {
        uint8_t value = testObject.code[i];
        TEST_ASSERT_EQUAL(i + 1, value);
    }

    TEST_ASSERT_EQUAL(8, testObject.capacity);
    TEST_ASSERT_EQUAL(8, testObject.count);

    writeChunk(&testObject, 9);
    TEST_ASSERT_EQUAL(16, testObject.capacity);
    TEST_ASSERT_EQUAL(9, testObject.count);
}

void testItShouldBeAbleToAddConstant()
{
    int index = addConstant(&testObject, wrapNumber(5.0));
    TEST_ASSERT_EQUAL(0, index);

    Value constantAt = getConstantAt(&testObject, index);
    TEST_ASSERT_EQUAL(5.0, unwrapNumber(constantAt));
}

void testItShouldBeAbleToWriteShort()
{
    writeShort(&testObject, UINT16_MAX);
    TEST_ASSERT_EQUAL(2, testObject.count);

    TEST_ASSERT_EQUAL(UINT8_MAX, testObject.code[0]);
    TEST_ASSERT_EQUAL(UINT8_MAX, testObject.code[1]);

    overwriteShort(&testObject, 0, 25);

    TEST_ASSERT_EQUAL(0, testObject.code[0]);
    TEST_ASSERT_EQUAL(25, testObject.code[1]);

    uint16_t asShort = readShort(&testObject, 0);
    TEST_ASSERT_EQUAL(25, asShort);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(testAddByteToChunk);
    RUN_TEST(testItShouldBeAbleToAddConstant);
    RUN_TEST(testItShouldBeAbleToWriteShort);
    return UNITY_END();
}