#ifndef VM_HEADER
#define VM_HEADER

#include "chunk.h"
#include "vm.h"
#include "hashmap.h"
#include <stdint.h>
#include <stdlib.h>

typedef struct VirtualMachine {
    Value stack[256];
    int currentStackIndex;
    void (*onStdOut) (char*);
    HashMap global;
    uint8_t* ip;
} VirtualMachine;

void initVirtualMachine(VirtualMachine*);
void interpret(VirtualMachine*, Chunk*);

void push(VirtualMachine*, Value);
Value peek(VirtualMachine*);
Value pop(VirtualMachine*);

#endif
