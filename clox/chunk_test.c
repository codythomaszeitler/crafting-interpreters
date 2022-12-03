#include "unity.h"
#include <stdio.h>
#include "chunk.h"

void setUp() {

}

void tearDown() {

}

void testInitAndAddToChunk() {
    Chunk testObject;
    initChunk(&testObject);
    TEST_ASSERT_EQUAL(0, testObject.count);
    TEST_ASSERT_EQUAL(0, testObject.capacity);
    TEST_ASSERT_NULL(testObject.code);
}

void testAddByteToChunk() {
    Chunk testObject;
    initChunk(&testObject);
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

    freeChunk(&testObject);
    TEST_ASSERT_EQUAL(0, testObject.capacity);
    TEST_ASSERT_EQUAL(0, testObject.count);
    TEST_ASSERT_EQUAL(NULL, testObject.code);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testInitAndAddToChunk);
    RUN_TEST(testAddByteToChunk);
    return UNITY_END();
}