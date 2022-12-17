#include "unity.h"
#include "bsa.h"
#include "stdlib.h"

BlockStaticAnalysis* testObject;
void setUp() {
    testObject = malloc(sizeof(BlockStaticAnalysis));
    initBlockStaticAnalysis(testObject);
}

void tearDown() {
    freeBlockStaticAnalysis(testObject);
}

static void testItShouldBeAbleToAddVariableBinding()
{
    addLocalVariableBinding(testObject, "a", 0);
    int offset = getLocalVariableBinding(testObject, "a");
    TEST_ASSERT_EQUAL(0, offset);

    BlockStaticAnalysis* oneLevelDeep = malloc(sizeof(BlockStaticAnalysis));
    initBlockStaticAnalysis(oneLevelDeep);
    addBlockTo(testObject, oneLevelDeep);

    TEST_ASSERT_EQUAL(testObject, oneLevelDeep->parent);
    TEST_ASSERT_EQUAL(testObject->blocks[0], oneLevelDeep);

    BlockStaticAnalysis* twoLevelsDeep = malloc(sizeof(BlockStaticAnalysis));
    initBlockStaticAnalysis(twoLevelsDeep);

    addBlockTo(oneLevelDeep, twoLevelsDeep);

    // So at this point, 2, would be the correct answer (I think?)
    findBindingDistance(twoLevelsDeep, "a");
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(testItShouldBeAbleToAddVariableBinding);
    return UNITY_END();
}