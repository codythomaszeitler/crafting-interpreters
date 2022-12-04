#ifndef VM_HEADER
#define VM_HEADER

#include "chunk.h"
#include "vm.h"

typedef struct VirtualMachine {
    Value stack[256];
    int currentStackIndex;
} VirtualMachine;

void initVirtualMachine(VirtualMachine*);
void interpret(VirtualMachine*, Chunk*);

void push(VirtualMachine*, Value);
Value peek(VirtualMachine*);
Value pop(VirtualMachine*);

#endif
