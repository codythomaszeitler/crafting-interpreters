#ifndef VM_HEADER
#define VM_HEADER

#include "chunk.h"
#include "vm.h"
#include "hashmap.h"
#include <stdint.h>
#include <stdlib.h>
#include "value.h"
#include "functionobj.h"

#define _NUM_CALL_FRAMES_ 256

typedef struct CallFrame
{
    ClosureObj *closure;
    uint8_t *ip;
    Value *sp;

    int currentStackIndex;
} CallFrame;

typedef struct VirtualMachine
{
    Value stack[256];
    void (*onStdOut)(char *);
    HashMap global;

    CallFrame frames[_NUM_CALL_FRAMES_];
    int fp;
    bool debugMode;

    UpvalueObj *upvalueHead;

} VirtualMachine;

void initVirtualMachine(VirtualMachine *);
void interpret(VirtualMachine *);

CallFrame* prepareForCall(VirtualMachine*, FunctionObj*);

void push(VirtualMachine *, Value);
Value peek(VirtualMachine *);
Value pop(VirtualMachine *);

#endif
