#ifndef VALUE_HEADER
#define VALUE_HEADER

#include "stdint.h"
#include <stdbool.h>
#include <math.h>
#include "object.h"

typedef enum ValueType {
    VALUE_TYPE_BOOL,
    VALUE_TYPE_NUMBER,
    VALUE_TYPE_OBJECT
} ValueType;

typedef struct Value {
    ValueType type;
    union Data {
        bool boolean;
        double number;
        Obj* object;
    } raw;
} Value;

typedef struct ValueArray {
    uint32_t count;
    uint32_t capacity;
    Value* constants;
} ValueArray;

void initValueArray(ValueArray*);
int writeValueArray(ValueArray*, Value);
void freeValueArray(ValueArray*);

Value getValueAt(ValueArray*, uint32_t);

bool unwrapBool(Value);
Value wrapBool(bool);
bool isBool(Value);

double unwrapNumber(Value);
Value wrapNumber(double number);
bool isNumber(Value);

Value wrapObject(Obj* pointer);
Obj* unwrapObject(Value);
bool isObject(Value);

bool equals(Value, Value);

Value add(Value, Value);

Value negate(Value);

#endif