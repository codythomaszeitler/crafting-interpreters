#include "unity.h"
#include "value.h"
#include <math.h>
#include "cloxstring.h"
#include <stdlib.h>

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
    Value toAdd = wrapNumber(5.0);
    int toAddConstantPoolIndex = writeValueArray(&testObject, toAdd);

    TEST_ASSERT_EQUAL(8, testObject.capacity);
    TEST_ASSERT_EQUAL(1, testObject.count);
    TEST_ASSERT_EQUAL(0, toAddConstantPoolIndex);

    TEST_ASSERT_EQUAL(5.0,  unwrapNumber(testObject.constants[0]));
    TEST_ASSERT_EQUAL(1, writeValueArray(&testObject, wrapNumber(6.0)));
    TEST_ASSERT_EQUAL(6.0, unwrapNumber(testObject.constants[1]));
}

void testItShouldBeAbleToGetValueAtExisting() {
    int index = writeValueArray(&testObject, wrapNumber(5.0));
    Value value = getValueAt(&testObject, index);
    TEST_ASSERT_EQUAL(5.0, unwrapNumber(value));
}

void testItShouldBeAbleToGetNaNBackIfNoExists() {
    Value value = getValueAt(&testObject, 0);
    TEST_ASSERT_FALSE(unwrapNumber(value) == unwrapNumber(value));
}

void testItShouldBeAbleToCreateString()
{
    const char* testCharArray = "This is a test string";

    StringObj* string = wrapString(testCharArray);
    char* characters = string->chars;

    TEST_ASSERT_EQUAL_STRING(characters, "This is a test string");
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testItShouldBeAbleToAddAValueToAConstantPool);
    RUN_TEST(testItShouldBeAbleToGetValueAtExisting);
    RUN_TEST(testItShouldBeAbleToGetNaNBackIfNoExists);
    RUN_TEST(testItShouldBeAbleToCreateString);
    return UNITY_END();
}