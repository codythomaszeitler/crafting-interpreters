#include "value.h"
#include "stdio.h"
#include "memory.h"
#include "math.h"
#include "stdint.h"
#include <stdbool.h>

void initValueArray(ValueArray* valueArray) {
    valueArray->count = 0;
    valueArray->capacity = 0;
    valueArray->constants = NULL;
}

int writeValueArray(ValueArray* valueArray, Value constant) {
    if (valueArray->capacity < valueArray->count + 1) {
        int oldCount = valueArray->capacity;
        valueArray->capacity = GROW_CAPACITY(valueArray->capacity);
        valueArray->constants = GROW_ARRAY(Value, valueArray->constants, oldCount, valueArray->capacity);
    }

    int constantIndex = valueArray->count;
    valueArray->constants[constantIndex] = constant;
    valueArray->count = constantIndex + 1;
    return constantIndex;
}

void freeValueArray(ValueArray* valueArray) {
    FREE_ARRAY(Value, valueArray->constants, valueArray->count);
    valueArray->count = 0;
    valueArray->capacity = 0;
    valueArray->constants = NULL;
}

Value getValueAt(ValueArray* valueArray, uint32_t index) {
    if ((index + 1) > valueArray->count) {
        return wrapNumber(NAN);
    }

    return valueArray->constants[index];
}

bool asBool(Value toUnwrap) {
    return false;
}

Value wrapNumber(double number) {
    Value asNumber;
    asNumber.raw.number = number;
    asNumber.type = VALUE_TYPE_NUMBER;
    return asNumber;
}

double unwrapNumber(Value value) {
    return value.raw.number;
}

bool isNumber(Value value) {
    return value.type == VALUE_TYPE_NUMBER;
}
