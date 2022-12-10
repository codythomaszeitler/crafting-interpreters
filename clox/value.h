#ifndef VALUE_HEADER
#define VALUE_HEADER

#include "stdint.h"
#include <stdbool.h>
#include <math.h>


typedef enum ValueType {
    VALUE_TYPE_BOOL,
    VALUE_TYPE_NUMBER
} ValueType;

typedef struct Value {
    ValueType type;
    union Data {
        bool boolean;
        double number;
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

bool asBool(Value);

double unwrapNumber(Value);
Value wrapNumber(double number);
bool isNumber(Value);

#endif