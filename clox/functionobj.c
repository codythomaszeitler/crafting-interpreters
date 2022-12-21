#include "functionobj.h"
#include "chunk.h"
#include <stdlib.h>
#include "object.h"

void initFunctionObj(FunctionObj *functionObj)
{
    Chunk *compiling = malloc(sizeof(Chunk));
    initChunk(compiling);

    functionObj->bytecode = compiling;
    functionObj->base.type = ObjFunction;
    functionObj->arity = 0;
}

void freeFunctionObj(FunctionObj *functionObj)
{
    freeChunk(functionObj->bytecode);
}

bool isFunctionObj(Value value)
{
    Obj *asBaseObject = unwrapObject(value);
    return asBaseObject->type == ObjFunction;
}

FunctionObj *unwrapFunctionObj(Value value)
{
    FunctionObj* compiledFunction = (FunctionObj*) unwrapObject(value);
    return compiledFunction;
}