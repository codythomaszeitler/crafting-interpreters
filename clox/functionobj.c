#include "functionobj.h"
#include "chunk.h"
#include <stdlib.h>
#include "object.h"
#include "memory.h"

void initFunctionObj(FunctionObj *functionObj)
{
    Chunk *compiling = malloc(sizeof(Chunk));
    initChunk(compiling);

    functionObj->bytecode = compiling;
    functionObj->base.type = ObjFunction;
    functionObj->arity = 0;

    functionObj->numUpvalues = 0;
    functionObj->upvalues = NULL;

    functionObj->numUpvalueCapacity = 0;
}

int addUpvalue(FunctionObj *function, Upvalue upvalue)
{

    if (function->numUpvalueCapacity < function->numUpvalues + 1)
    {
        int oldCapacity = function->numUpvalues;
        function->numUpvalueCapacity = GROW_CAPACITY(oldCapacity);
        function->upvalues = GROW_ARRAY(Upvalue, function->upvalues, oldCapacity, function->numUpvalueCapacity);
    }
    function->upvalues[function->numUpvalues] = upvalue;

    int numUpvalues = function->numUpvalues;
    function->numUpvalues++;
    return numUpvalues;
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
    FunctionObj *compiledFunction = (FunctionObj *)unwrapObject(value);
    return compiledFunction;
}

void initClosureObj(ClosureObj *closureObj)
{
    closureObj->base.type = ObjClosure;
    closureObj->function = NULL;
    closureObj->upvalues = NULL;
}

void freeClosureObj(ClosureObj *closureObj)
{
}

bool isClosureObj(Value value)
{
    Obj *asBaseObject = unwrapObject(value);
    return asBaseObject->type == ObjClosure;
}

ClosureObj *unwrapClosureObj(Value value)
{
    ClosureObj *closure = (ClosureObj *)unwrapObject(value);
    return closure;
}

void initUpvalueObj(UpvalueObj *upvalueObj)
{
}

int addUpvalueObj(FunctionObj *function, UpvalueObj *upvalue)
{
    return 0;
}