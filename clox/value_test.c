#include "unity.h"
#include "value.h"
#include <math.h>

ValueArray testObject;

void setUp() {
    initValueArray(&testObject);
    TEST_ASSERT_EQUAL(0, testObject.count);
    TEST_ASSERT_EQUAL(0, testObject.capacity);
}

void tearDown() {
    freeValueArray(&testObject);
    TEST_ASSERT_EQUAL(0, testObject.capacity);
    TEST_ASSERT_EQUAL(0, testObject.count);
    TEST_ASSERT_EQUAL(NULL, testObject.constants);
}

void testItShouldBeAbleToAddAValueToAConstantPool() {
    Value toAdd = 5.0;
    int toAddConstantPoolIndex = writeValueArray(&testObject, toAdd);

    TEST_ASSERT_EQUAL(8, testObject.capacity);
    TEST_ASSERT_EQUAL(1, testObject.count);
    TEST_ASSERT_EQUAL(0, toAddConstantPoolIndex);

    TEST_ASSERT_EQUAL(5.0, testObject.constants[0]);
    TEST_ASSERT_EQUAL(1, writeValueArray(&testObject, 6.0));
    TEST_ASSERT_EQUAL(6.0, testObject.constants[1]);
}

void testItShouldBeAbleToGetValueAtExisting() {
    int index = writeValueArray(&testObject, 5.0);
    Value value = getValueAt(&testObject, index);
    TEST_ASSERT_EQUAL(5.0, value);
}

void testItShouldBeAbleToGetNaNBackIfNoExists() {
    Value value = getValueAt(&testObject, 0);
    TEST_ASSERT_FALSE(value == value);
}

void testItShouldBeAbleToGetNaNIfNegativeIndex() {
    Value value = getValueAt(&testObject, 0);
    TEST_ASSERT_FALSE(value == value);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testItShouldBeAbleToGetValueAtExisting);
    RUN_TEST(testItShouldBeAbleToAddAValueToAConstantPool);
    RUN_TEST(testItShouldBeAbleToGetNaNBackIfNoExists);
    RUN_TEST(testItShouldBeAbleToGetNaNIfNegativeIndex);
    return UNITY_END();
}