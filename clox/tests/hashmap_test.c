#include "unity.h"
#include "hashmap.h"
#include "value.h"
#include "cloxstring.h"

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
    // But tath doesn't make sense either since wrap string is not a 
    hashMapPut(&testObject, asString("a"), wrapNumber(5.0));
    Value aValue = hashMapGet(&testObject, asString("a"));
    TEST_ASSERT_EQUAL(5.0, unwrapNumber(aValue));

    Value bValue = hashMapGet(&testObject, asString("b"));
    TEST_ASSERT_EQUAL(6.0, unwrapNumber(bValue));

    hashMapPut(&testObject, asString("a"), wrapNumber(3.0));
    Value overriddenAValue = hashMapGet(&testObject, asString("a"));
    TEST_ASSERT_EQUAL(3.0, unwrapNumber(overriddenAValue));

    TEST_ASSERT_EQUAL(2, hashMapSize(&testObject));
}

void testItShouldGetNilIfNoKeyExists() {
    Value nil = hashMapGet(&testObject, asString("a"));
    TEST_ASSERT_TRUE(isNil(nil));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(testItShouldPutValueInWithKey);
    RUN_TEST(testItShouldGetNilIfNoKeyExists);
    return UNITY_END();
}