#include "unity.h"
#include <stdio.h>

void setUp() {

}

void tearDown() {

}

void testFunction() {
    printf("Hello, World!");
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testFunction);
    return UNITY_END();
}