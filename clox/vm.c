#include "vm.h"
#include "value.h"
#include "chunk.h"
#include <stdint.h>

void initVirtualMachine(VirtualMachine *vm)
{
    vm->currentStackIndex = 0;
}

void interpret(VirtualMachine *vm, Chunk *bytecode)
{
    uint64_t iterator = 0;
    while (iterator < bytecode->count)
    {
        OpCode opCode = bytecode->code[iterator];
        if (opCode == OP_CONSTANT)
        {
            int constantIndex = bytecode->code[iterator + 1];
            Value constant = getConstantAt(bytecode, constantIndex);
            push(vm, constant);

            iterator = iterator + getByteLengthFor(OP_CONSTANT);
        }
        else if (opCode == OP_NEGATE)
        {
            Value number = pop(vm);
            double value = unwrapNumber(number);

            Value result = wrapNumber(-1 * value);
            push(vm, result);

            iterator = iterator + 1;
        }
        else if (opCode == OP_ADD)
        {
            Value rightValue = pop(vm);
            Value leftValue = pop(vm);

            double right = unwrapNumber(rightValue);
            double left = unwrapNumber(leftValue);

            Value result = wrapNumber(left + right);
            push(vm, result);

            iterator = iterator + getByteLengthFor(OP_ADD);
        }
        else if (opCode == OP_MULT)
        {
            Value rightValue = pop(vm);
            Value leftValue = pop(vm);

            double right = unwrapNumber(rightValue);
            double left = unwrapNumber(leftValue);

            Value result = wrapNumber(left * right);
            push(vm, result);

            iterator = iterator + getByteLengthFor(opCode);
        }
        else if (opCode == OP_DIV)
        {
            Value rightValue = pop(vm);
            Value leftValue = pop(vm);

            double right = unwrapNumber(rightValue);
            double left = unwrapNumber(leftValue);
            Value result = wrapNumber(left / right);
            push(vm, result);

            iterator = iterator + getByteLengthFor(opCode);
        }
        else if (opCode == OP_SUB)
        {
            Value rightValue = pop(vm);
            Value leftValue = pop(vm);

            double right = unwrapNumber(rightValue);
            double left = unwrapNumber(leftValue);
            Value result = wrapNumber(left - right);
            push(vm, result);

            iterator = iterator + getByteLengthFor(opCode);
        }
        else if (opCode == OP_TRUE)
        {
        }
        else
        {
            iterator = iterator + 1;
        }
    }
}

Value peek(VirtualMachine *vm)
{
    int stackPeekIndex = vm->currentStackIndex - 1;
    return vm->stack[stackPeekIndex];
}

void push(VirtualMachine *vm, Value value)
{
    vm->stack[vm->currentStackIndex] = value;
    vm->currentStackIndex++;
}

Value pop(VirtualMachine *vm)
{
    int stackPeekIndex = vm->currentStackIndex - 1;
    Value popped = vm->stack[stackPeekIndex];
    vm->currentStackIndex--;
    return popped;
}