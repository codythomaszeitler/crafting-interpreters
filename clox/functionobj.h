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

typedef struct ClosureObj {
    Obj base;
    FunctionObj* function;
} ClosureObj;

void initFunctionObj(FunctionObj*);
void freeFunctionObj(FunctionObj*);

bool isFunctionObj(Value);
FunctionObj* unwrapFunctionObj(Value);

// Should init closure always take in a function, I mean you HAVE to have one right?
void initClosureObj(ClosureObj*);
void freeClosureObj(ClosureObj*);

bool isClosureObj(Value);
ClosureObj* unwrapClosureObj(Value);


#endif