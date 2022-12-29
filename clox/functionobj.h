#ifndef FUNCTION_OBJ_HEADER
#define FUNCTION_OBJ_HEADER

#include "object.h"
#include "chunk.h"
#include "cloxstring.h"
typedef struct Upvalue
{
    bool isLocal;
    uint8_t index;
} Upvalue;

typedef struct FunctionObj
{
    Obj base;
    Chunk *bytecode;
    StringObj *name;
    int arity;

    Upvalue *upvalues;
    int numUpvalues;
    int numUpvalueCapacity;
} FunctionObj;

typedef struct UpvalueObj
{
    Obj base;
    Value *location;
    Value closed;

    struct UpvalueObj *next;
} UpvalueObj;

typedef struct ClosureObj
{
    Obj base;
    FunctionObj *function;
    UpvalueObj **upvalues;

} ClosureObj;

void initFunctionObj(FunctionObj *);
void freeFunctionObj(FunctionObj *);

bool isFunctionObj(Value);
FunctionObj *unwrapFunctionObj(Value);
int addUpvalue(FunctionObj *, Upvalue);

// Should init closure always take in a function, I mean you HAVE to have one right?
void initClosureObj(ClosureObj *);
void freeClosureObj(ClosureObj *);

bool isClosureObj(Value);
ClosureObj *unwrapClosureObj(Value);

void initUpvalueObj(UpvalueObj *);

int addUpvalueObj(FunctionObj *, UpvalueObj*);

#endif