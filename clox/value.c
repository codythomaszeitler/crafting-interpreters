#include "value.h"
#include "stdio.h"
#include "memory.h"
#include "math.h"
#include "stdint.h"
#include <stdbool.h>
#include <string.h>
#include "cloxstring.h"
#include <stdlib.h>

void initValueArray(ValueArray *valueArray)
{
    valueArray->count = 0;
    valueArray->capacity = 0;
    valueArray->constants = NULL;
}

int writeValueArray(ValueArray *valueArray, Value constant)
{
    if (valueArray->capacity < valueArray->count + 1)
    {
        int oldCount = valueArray->capacity;
        valueArray->capacity = GROW_CAPACITY(valueArray->capacity);
        valueArray->constants = GROW_ARRAY(Value, valueArray->constants, oldCount, valueArray->capacity);
    }

    int constantIndex = valueArray->count;
    valueArray->constants[constantIndex] = constant;
    valueArray->count = constantIndex + 1;
    return constantIndex;
}

void freeValueArray(ValueArray *valueArray)
{
    FREE_ARRAY(Value, valueArray->constants, valueArray->count);
    valueArray->count = 0;
    valueArray->capacity = 0;
    valueArray->constants = NULL;
}

Value getValueAt(ValueArray *valueArray, uint32_t index)
{
    if ((index + 1) > valueArray->count)
    {
        return wrapNumber(NAN);
    }

    return valueArray->constants[index];
}

bool unwrapBool(Value value)
{
    return value.raw.boolean;
}

Value wrapBool(bool boolean)
{
    Value asBool;
    asBool.raw.boolean = boolean;
    asBool.type = VALUE_TYPE_BOOL;
    return asBool;
}

bool isBool(Value value)
{
    return value.type == VALUE_TYPE_BOOL;
}

Value wrapNumber(double number)
{
    Value asNumber;
    asNumber.raw.number = number;
    asNumber.type = VALUE_TYPE_NUMBER;
    return asNumber;
}

double unwrapNumber(Value value)
{
    return value.raw.number;
}

bool isNumber(Value value)
{
    return value.type == VALUE_TYPE_NUMBER;
}

Value wrapObject(Obj *pointer)
{
    Value asObject;
    asObject.raw.object = pointer;
    asObject.type = VALUE_TYPE_OBJECT;
    return asObject;
}

Obj *unwrapObject(Value value)
{
    return value.raw.object;
}

bool isObject(Value value)
{
    return value.type == VALUE_TYPE_OBJECT;
}

bool isNil(Value value) {
    return value.type == VALUE_TYPE_NIL;
}

Value nil() {
    Value nil;
    nil.type = VALUE_TYPE_NIL;
    return nil;
}

bool equals(Value left, Value right)
{
    return left.raw.boolean == right.raw.boolean;
}

Value negate(Value value)
{
    if (isBool(value))
    {
        bool result = !value.raw.boolean;
        return wrapBool(result);
    }
    else
    {
        double result = (-1 * value.raw.number);
        return wrapNumber(result);
    }
}

Value add(Value leftValue, Value rightValue)
{
    if (isNumber(leftValue) && isNumber(rightValue))
    {
        double right = unwrapNumber(rightValue);
        double left = unwrapNumber(leftValue);

        Value result = wrapNumber(left + right);
        return result;
    }
    else
    {
        StringObj *right = (StringObj *)unwrapObject(rightValue);
        StringObj *left = (StringObj *)unwrapObject(leftValue);

        int length = right->length + left->length;
        char* concatenated = malloc(length + 1);
        snprintf(concatenated, length + 1, "%s%s", left->chars, right->chars);
        concatenated[length] = '\0';

        StringObj* concat = malloc(sizeof(StringObj));
        concat->type.type = 1;
        concat->length = length;
        concat->chars = concatenated;

        freeStringObj(right);
        freeStringObj(left);

        return wrapObject((Obj*)concat);
    }
}