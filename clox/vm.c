#include "vm.h"
#include "value.h"
#include "chunk.h"
#include <stdint.h>
#include "cloxstring.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

static void stdOutPrinter(char *toPrint)
{
    printf(toPrint);
}

void initVirtualMachine(VirtualMachine *vm)
{
    vm->currentStackIndex = 0;
    vm->onStdOut = stdOutPrinter;
    vm->ip = NULL;

    initHashMap(&vm->global);
}

static void interpretConstant(VirtualMachine *vm, Chunk *bytecode)
{
    int constantIndex = *(vm->ip + 1);
    Value constant = getConstantAt(bytecode, constantIndex);
    push(vm, constant);

    vm->ip = vm->ip + getByteLengthFor(OP_CONSTANT);
}

static void interpretNegate(VirtualMachine *vm, Chunk *bytecode)
{
    Value value = pop(vm);
    push(vm, negate(value));

    vm->ip = vm->ip + getByteLengthFor(OP_NEGATE);
}

static void interpretAdd(VirtualMachine *vm, Chunk *bytecode)
{
    Value rightValue = pop(vm);
    Value leftValue = pop(vm);

    Value result = add(leftValue, rightValue);
    push(vm, result);

    vm->ip = vm->ip + getByteLengthFor(OP_ADD);
}

static void interpretMultiplication(VirtualMachine *vm, Chunk *bytecode)
{
    Value rightValue = pop(vm);
    Value leftValue = pop(vm);

    double right = unwrapNumber(rightValue);
    double left = unwrapNumber(leftValue);

    Value result = wrapNumber(left * right);
    push(vm, result);

    vm->ip = vm->ip + getByteLengthFor(OP_MULT);
}

static void interpretDivision(VirtualMachine *vm, Chunk *bytecode)
{
    Value rightValue = pop(vm);
    Value leftValue = pop(vm);

    double right = unwrapNumber(rightValue);
    double left = unwrapNumber(leftValue);
    Value result = wrapNumber(left / right);
    push(vm, result);

    vm->ip = vm->ip + getByteLengthFor(OP_DIV);
}

static void interpretSubtraction(VirtualMachine *vm, Chunk *bytecode)
{
    Value rightValue = pop(vm);
    Value leftValue = pop(vm);

    double right = unwrapNumber(rightValue);
    double left = unwrapNumber(leftValue);
    Value result = wrapNumber(left - right);
    push(vm, result);

    vm->ip = vm->ip + getByteLengthFor(OP_SUB);
}

static void interpretTrue(VirtualMachine *vm, Chunk *bytecode)
{
    Value boolean = wrapBool(true);
    push(vm, boolean);
    vm->ip = vm->ip + getByteLengthFor(OP_TRUE);
}

static void interpretFalse(VirtualMachine *vm, Chunk *bytecode)
{
    Value boolean = wrapBool(false);
    push(vm, boolean);
    vm->ip = vm->ip + getByteLengthFor(OP_FALSE);
}

static void interpretEquals(VirtualMachine *vm, Chunk *bytecode)
{
    Value right = pop(vm);
    Value left = pop(vm);
    Value result = wrapBool(equals(left, right));
    push(vm, result);

    vm->ip = vm->ip + getByteLengthFor(OP_EQUAL);
}

static void interpretLessThan(VirtualMachine *vm, Chunk *bytecode)
{
    Value rightValue = pop(vm);
    Value leftValue = pop(vm);

    double right = unwrapNumber(rightValue);
    double left = unwrapNumber(leftValue);
    Value result = wrapBool(left < right);
    push(vm, result);

    vm->ip = vm->ip + getByteLengthFor(OP_LESS_THAN);
}

static void interpretString(VirtualMachine *vm, Chunk *bytecode)
{
    char *chars = (char *)(vm->ip + 1);

    Value string = wrapString(chars);
    StringObj *underlying = (StringObj *)unwrapObject(string);

    push(vm, string);

    // It needs to be one extra for the null terminated string.
    vm->ip = vm->ip + underlying->length + 2;
}

static void interpretPrint(VirtualMachine *vm, Chunk *bytecode)
{
    char *line = NULL;

    Value expression = pop(vm);
    if (isNumber(expression))
    {
        int length = snprintf(NULL, 0, "%f", unwrapNumber(expression));
        line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%f", unwrapNumber(expression));
        line[length] = '\0';
    }
    else if (isBool(expression))
    {
        bool asBool = unwrapBool(expression);
        int length = snprintf(NULL, 0, "%s", asBool ? "true" : "false");
        line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%s", asBool ? "true" : "false");
        line[length] = '\0';
    }
    else if (isStringObj(expression))
    {
        StringObj* unwrapped = (StringObj*)unwrapObject(expression);
        int length = snprintf(NULL, 0, "%s", unwrapped->chars);
        line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%s", unwrapped->chars);
    }

    if (vm->onStdOut != NULL)
    {
        vm->onStdOut(line);
    }
    vm->ip = vm->ip + 1;
}

static void interpretVarDecl(VirtualMachine *vm, Chunk *bytecode)
{
    push(vm, nil());
    vm->ip = vm->ip + getByteLengthFor(OP_VAR_DECL);
}

static void interpretVarAssign(VirtualMachine *vm, Chunk *bytecode)
{
    uint8_t offset = *(vm->ip + 1);
    vm->stack[offset] = pop(vm);
    vm->ip = vm->ip + 2;
}

static void interpretVarExpression(VirtualMachine *vm, Chunk *bytecode)
{
    uint8_t offset = *(vm->ip + 1);
    Value value = vm->stack[offset];
    push(vm, value);

    vm->ip = vm->ip + 2;
}

static void interpretGlobalVarDecl(VirtualMachine *vm, Chunk *bytecode)
{
    int constantIndex = *(vm->ip + 1);
    Value constant = getConstantAt(bytecode, constantIndex);
    StringObj *asString = (StringObj *)unwrapObject(constant);
    hashMapPut(&vm->global, asString, nil());

    vm->ip = vm->ip + 2;
}

static void interpretGlobalVarAssign(VirtualMachine *vm, Chunk *bytecode)
{
    int constantIndex = *(vm->ip + 1);
    Value constant = getConstantAt(bytecode, constantIndex);
    StringObj *asString = (StringObj *)unwrapObject(constant);

    hashMapPut(&vm->global, asString, pop(vm));
    vm->ip = vm->ip + 2;
}

static void interpretGlobalExpression(VirtualMachine *vm, Chunk *bytecode)
{
    int constantIndex = *(vm->ip + 1);
    Value constant = getConstantAt(bytecode, constantIndex);
    StringObj *asString = (StringObj *)unwrapObject(constant);

    push(vm, hashMapGet(&vm->global, asString));

    vm->ip = vm->ip + 2;
}

static void interpretPop(VirtualMachine *vm, Chunk *bytecode)
{
    pop(vm);
    vm->ip = vm->ip + 1;
}

static void interpretJumpIfFalse(VirtualMachine *vm, Chunk *bytecode)
{
    Value expression = pop(vm);
    bool unwrapped = unwrapBool(expression);

    if (!unwrapped)
    {
        uint8_t msb = *(vm->ip + 1);
        uint8_t lsb = *(vm->ip + 2);
        uint16_t jumpLocation = (msb << 8) | lsb;
        vm->ip = &bytecode->code[jumpLocation];
    }
    else
    {
        vm->ip = vm->ip + 3;
    }
}

static void interpretLoop(VirtualMachine *vm, Chunk *bytecode)
{
    uint8_t jumpOffset = *(vm->ip + 1);
    vm->ip = vm->ip - (jumpOffset);
}

static void interpretJump(VirtualMachine *vm, Chunk *bytecode)
{
    uint8_t msb = *(vm->ip + 1);
    uint8_t lsb = *(vm->ip + 2);
    uint16_t jumpLocation = (msb << 8) | lsb;
    vm->ip = &bytecode->code[jumpLocation];
}

static bool isAtEndOfBytecode(VirtualMachine *vm, Chunk *bytecode)
{
    OpCode opCode = *(vm->ip);
    return OP_RETURN == opCode;
}

void interpret(VirtualMachine *vm, Chunk *bytecode)
{
    vm->ip = bytecode->code;

    while (!isAtEndOfBytecode(vm, bytecode))
    {
        OpCode opCode = *(vm->ip);
        switch (opCode)
        {
        case OP_CONSTANT:
            interpretConstant(vm, bytecode);
            break;
        case OP_NEGATE:
            interpretNegate(vm, bytecode);
            break;
        case OP_ADD:
            interpretAdd(vm, bytecode);
            break;
        case OP_MULT:
            interpretMultiplication(vm, bytecode);
            break;
        case OP_DIV:
            interpretDivision(vm, bytecode);
            break;
        case OP_SUB:
            interpretSubtraction(vm, bytecode);
            break;
        case OP_LESS_THAN:
            interpretLessThan(vm, bytecode);
            break;
        case OP_TRUE:
            interpretTrue(vm, bytecode);
            break;
        case OP_FALSE:
            interpretFalse(vm, bytecode);
            break;
        case OP_EQUAL:
            interpretEquals(vm, bytecode);
            break;
        case OP_STRING:
            interpretString(vm, bytecode);
            break;
        case OP_PRINT:
            interpretPrint(vm, bytecode);
            break;
        case OP_VAR_DECL:
            interpretVarDecl(vm, bytecode);
            break;
        case OP_VAR_ASSIGN:
            interpretVarAssign(vm, bytecode);
            break;
        case OP_VAR_EXPRESSION:
            interpretVarExpression(vm, bytecode);
            break;
        case OP_VAR_GLOBAL_DECL:
            interpretGlobalVarDecl(vm, bytecode);
            break;
        case OP_VAR_GLOBAL_ASSIGN:
            interpretGlobalVarAssign(vm, bytecode);
            break;
        case OP_VAR_GLOBAL_EXPRESSION:
            interpretGlobalExpression(vm, bytecode);
            break;
        case OP_POP:
            interpretPop(vm, bytecode);
            break;
        case OP_JUMP_IF_FALSE:
            interpretJumpIfFalse(vm, bytecode);
            break;
        case OP_LOOP:
            interpretLoop(vm, bytecode);
            break;
        case OP_JUMP:
            interpretJump(vm, bytecode);
            break;
        default:
            printf("Invalid op code.");
            break;
        }
    }
    if (isAtEndOfBytecode(vm, bytecode))
    {
        vm->ip = vm->ip + 1;
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