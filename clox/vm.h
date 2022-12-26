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
    FunctionObj *function;
    uint8_t *ip;
    // This is the start of the usable stack. 
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

    int currentStackIndex;
} VirtualMachine;

void initVirtualMachine(VirtualMachine *);
void interpret(VirtualMachine *);

// So given a function object, it should be able to set up a call stack for a function object?
// So you are about to call a function, you need to 

CallFrame* prepareForCall(VirtualMachine*, FunctionObj*);

void push(VirtualMachine *, Value);
Value peek(VirtualMachine *);
Value pop(VirtualMachine *);

#endif
