// So it looks like if you are going to run a test suite,
// you are going to have a completely different main function?
// I wonder if JUnit does something like this?
// Very interesting...
#include "chunk.h"
#include <stdio.h>
int main() {
    helloWorld();
    printf("Hello, World!");
    return 0;
}