#include "unity.h"
#include "hashmap.h"
#include "value.h"

HashMap testObject;
void setUp()
{
    initHashMap(&testObject);
}

void tearDown()
{
    freeHashMap(&testObject);
}

void testItShouldPutValueInWithKey()
{
    hashMapPut(&testObject, "a", wrapNumber(5.0));
    Value aValue = hashMapGet(&testObject, "a");
    TEST_ASSERT_EQUAL(5.0, unwrapNumber(aValue));

    hashMapPut(&testObject, "b", wrapNumber(6.0));
    Value bValue = hashMapGet(&testObject, "b");
    TEST_ASSERT_EQUAL(6.0, unwrapNumber(bValue));

    hashMapPut(&testObject, "a", wrapNumber(3.0));
    Value overriddenAValue = hashMapGet(&testObject, "a");
    TEST_ASSERT_EQUAL(3.0, unwrapNumber(overriddenAValue));

    TEST_ASSERT_EQUAL(2, hashMapSize(&testObject));
}

void testItShouldGetNilIfNoKeyExists() {
    Value nil = hashMapGet(&testObject, "a");
    TEST_ASSERT_TRUE(isNil(nil));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(testItShouldPutValueInWithKey);
    RUN_TEST(testItShouldGetNilIfNoKeyExists);
    return UNITY_END();
}