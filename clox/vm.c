#include "vm.h"
#include "value.h"
#include "chunk.h"
#include <stdint.h>

void initVirtualMachine(VirtualMachine* vm) {
    vm->currentStackIndex = 0;
}

void interpret(VirtualMachine* vm, Chunk* bytecode) {
    uint64_t iterator = 0;
    while (iterator < bytecode->count) {
        OpCode opCode = bytecode->code[iterator];
        if (opCode == OP_CONSTANT) {
            int constantIndex = bytecode->code[iterator + 1];
            Value constant = getConstantAt(bytecode, constantIndex);
            push(vm, constant);

            iterator = iterator + 2;
        } else if (opCode == OP_NEGATE) {
            Value value = pop(vm);
            push(vm, (-1 * value));

            iterator = iterator + 1;
        } else if (opCode == OP_ADD) {
            Value right = pop(vm);
            Value left = pop(vm);

            Value result = left + right;
            push(vm, result);

            iterator = iterator + 1;
        } else {
            iterator = iterator + 1;
        }
    }
}

Value peek(VirtualMachine* vm) {
    int stackPeekIndex = vm->currentStackIndex - 1;
    return vm->stack[stackPeekIndex];
}

void push(VirtualMachine* vm, Value value) {
    vm->stack[vm->currentStackIndex] = value;
    vm->currentStackIndex++;
}

Value pop(VirtualMachine* vm) {
    int stackPeekIndex = vm->currentStackIndex - 1;
    Value popped = vm->stack[stackPeekIndex];
    vm->currentStackIndex--;
    return popped;
}