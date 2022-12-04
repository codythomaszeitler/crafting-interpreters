#ifndef VALUE_HEADER
#define VALUE_HEADER

#include "stdint.h"
#include <math.h>

typedef double Value;

typedef struct ValueArray {
    uint32_t count;
    uint32_t capacity;
    Value* constants;
} ValueArray;

void initValueArray(ValueArray*);
int writeValueArray(ValueArray*, Value);
void freeValueArray(ValueArray*);

Value getValueAt(ValueArray*, uint32_t);

#endif