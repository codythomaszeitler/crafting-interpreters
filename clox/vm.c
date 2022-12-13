#include "vm.h"
#include "value.h"
#include "chunk.h"
#include <stdint.h>
#include "cloxstring.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void initVirtualMachine(VirtualMachine *vm)
{
    vm->currentStackIndex = 0;
    vm->onStdOut = NULL;

    // How can our VM
    // Our VM would be running in a certain context, right?
    initHashMap(&vm->global);
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
            Value value = pop(vm);
            push(vm, negate(value));

            iterator = iterator + 1;
        }
        else if (opCode == OP_ADD)
        {
            Value rightValue = pop(vm);
            Value leftValue = pop(vm);

            Value result = add(leftValue, rightValue);
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
            Value boolean = wrapBool(true);
            push(vm, boolean);
            iterator = iterator + getByteLengthFor(opCode);
        }
        else if (opCode == OP_FALSE)
        {
            Value boolean = wrapBool(false);
            push(vm, boolean);
            iterator = iterator + getByteLengthFor(opCode);
        }
        else if (opCode == OP_EQUAL)
        {
            Value right = pop(vm);
            Value left = pop(vm);
            Value result = wrapBool(equals(left, right));
            push(vm, result);
            iterator = iterator + getByteLengthFor(opCode);
        }
        else if (opCode == OP_STRING)
        {

            char *chars = (char *)&bytecode->code[iterator + 1];

            StringObj *stringObj = wrapString(chars);
            Value reference = wrapObject((Obj *)stringObj);

            push(vm, reference);

            iterator = iterator + 1 + stringObj->length + 1;
        }
        else if (opCode == OP_PRINT)
        {
            Value expression = pop(vm);
            int length = snprintf(NULL, 0, "%f", unwrapNumber(expression));

            char *line = malloc(sizeof(char) * length + 1);
            snprintf(line, length + 1, "%f", unwrapNumber(expression));
            line[length] = '\0';

            vm->onStdOut(line);

            iterator = iterator + 1;
        }
        else if (opCode == OP_VAR_DECL)
        {
            char *chars = (char *)&bytecode->code[iterator + 1];
            hashMapPut(&vm->global, chars, nil());

            iterator = iterator + strlen(chars) + 1 + 1;
        }
        else if (opCode == OP_VAR_ASSIGN) {
            char *chars = (char*) &bytecode->code[iterator + 1];
            Value value = pop(vm);
            hashMapPut(&vm->global, chars, value);

            iterator = iterator + strlen(chars) + 1 + 1;
        }
        else if (opCode == OP_VAR_EXPRESSION) {
            char *chars = (char*) &bytecode->code[iterator + 1];
            Value value = hashMapGet(&vm->global, chars);
            push(vm, value);

            iterator = iterator + strlen(chars) + 1 + 1;
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