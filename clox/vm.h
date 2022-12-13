#ifndef VM_HEADER
#define VM_HEADER

#include "chunk.h"
#include "vm.h"
#include "hashmap.h"

typedef struct VirtualMachine {
    Value stack[256];
    int currentStackIndex;
    void (*onStdOut) (char*);
    HashMap global;
} VirtualMachine;

void initVirtualMachine(VirtualMachine*);
void interpret(VirtualMachine*, Chunk*);

void push(VirtualMachine*, Value);
Value peek(VirtualMachine*);
Value pop(VirtualMachine*);

#endif
