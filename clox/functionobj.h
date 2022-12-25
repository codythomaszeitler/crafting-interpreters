#ifndef FUNCTION_OBJ_HEADER
#define FUNCTION_OBJ_HEADER

#include "object.h"
#include "chunk.h"
#include "cloxstring.h"

typedef struct FunctionObj {
    Obj base;
    Chunk* bytecode;
    StringObj* name;
    int arity;
} FunctionObj;

void initFunctionObj(FunctionObj*);
void freeFunctionObj(FunctionObj*);

bool isFunctionObj(Value);
FunctionObj* unwrapFunctionObj(Value);

#endif