#include "vm.h"
#include "value.h"
#include "chunk.h"
#include <stdint.h>
#include "cloxstring.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "disassembler.h"

static void stdOutPrinter(char *toPrint)
{
    printf(toPrint);
}

static CallFrame *getNextFrame(VirtualMachine *vm)
{
    int nextFrameIndex = vm->fp + 1;
    return &vm->frames[nextFrameIndex];
}

static CallFrame *getCurrentFrame(VirtualMachine *vm)
{
    return &vm->frames[vm->fp];
}

static uint8_t *getCurrentIp(VirtualMachine *vm)
{
    CallFrame *currentFrame = getCurrentFrame(vm);
    return currentFrame->ip;
}

static Value getCurrentFrameConstantAt(VirtualMachine *vm, int index)
{
    CallFrame *currentFrame = getCurrentFrame(vm);
    Value constant = getConstantAt(currentFrame->closure->function->bytecode, index);
    return constant;
}

void initVirtualMachine(VirtualMachine *vm)
{
    vm->onStdOut = stdOutPrinter;
    vm->fp = -1;
    vm->upvalueHead = NULL;

    initHashMap(&vm->global);

    for (int i = 0; i < _NUM_CALL_FRAMES_; i++)
    {
        vm->frames[i].ip = NULL;
        vm->frames[i].closure = NULL;
        vm->frames[i].sp = NULL;
    }
}

static void interpretConstant(VirtualMachine *vm)
{
    CallFrame *currentFrame = getCurrentFrame(vm);
    int constantIndex = *(currentFrame->ip + 1);
    Value constant = getCurrentFrameConstantAt(vm, constantIndex);
    push(vm, constant);

    currentFrame->ip = currentFrame->ip + getByteLengthFor(OP_CONSTANT);
}

static void interpretNegate(VirtualMachine *vm)
{
    Value value = pop(vm);
    push(vm, negate(value));

    vm->frames[vm->fp].ip = vm->frames[vm->fp].ip + getByteLengthFor(OP_NEGATE);
}

static void interpretAdd(VirtualMachine *vm)
{
    Value rightValue = pop(vm);
    Value leftValue = pop(vm);

    Value result = add(leftValue, rightValue);
    push(vm, result);

    vm->frames[vm->fp].ip = vm->frames[vm->fp].ip + getByteLengthFor(OP_ADD);
}

static void interpretMultiplication(VirtualMachine *vm)
{
    Value rightValue = pop(vm);
    Value leftValue = pop(vm);

    double right = unwrapNumber(rightValue);
    double left = unwrapNumber(leftValue);

    Value result = wrapNumber(left * right);
    push(vm, result);

    vm->frames[vm->fp].ip = vm->frames[vm->fp].ip + getByteLengthFor(OP_MULT);
}

static void interpretDivision(VirtualMachine *vm)
{
    Value rightValue = pop(vm);
    Value leftValue = pop(vm);

    double right = unwrapNumber(rightValue);
    double left = unwrapNumber(leftValue);
    Value result = wrapNumber(left / right);
    push(vm, result);

    vm->frames[vm->fp].ip = vm->frames[vm->fp].ip + getByteLengthFor(OP_DIV);
}

static void interpretSubtraction(VirtualMachine *vm)
{
    Value rightValue = pop(vm);
    Value leftValue = pop(vm);

    double right = unwrapNumber(rightValue);
    double left = unwrapNumber(leftValue);
    Value result = wrapNumber(left - right);
    push(vm, result);

    vm->frames[vm->fp].ip = vm->frames[vm->fp].ip + getByteLengthFor(OP_SUB);
}

static void interpretTrue(VirtualMachine *vm)
{
    Value boolean = wrapBool(true);
    push(vm, boolean);
    vm->frames[vm->fp].ip = vm->frames[vm->fp].ip + getByteLengthFor(OP_TRUE);
}

static void interpretFalse(VirtualMachine *vm)
{
    Value boolean = wrapBool(false);
    push(vm, boolean);
    vm->frames[vm->fp].ip = vm->frames[vm->fp].ip + getByteLengthFor(OP_FALSE);
}

static void interpretEquals(VirtualMachine *vm)
{
    Value right = pop(vm);
    Value left = pop(vm);
    Value result = wrapBool(equals(left, right));
    push(vm, result);

    vm->frames[vm->fp].ip = vm->frames[vm->fp].ip + getByteLengthFor(OP_EQUAL);
}

static void interpretLessThan(VirtualMachine *vm)
{
    Value rightValue = pop(vm);
    Value leftValue = pop(vm);

    double right = unwrapNumber(rightValue);
    double left = unwrapNumber(leftValue);
    Value result = wrapBool(left < right);
    push(vm, result);

    vm->frames[vm->fp].ip = vm->frames[vm->fp].ip + getByteLengthFor(OP_LESS_THAN);
}

static void interpretString(VirtualMachine *vm)
{
    char *chars = (char *)(vm->frames[vm->fp].ip + 1);

    Value string = wrapString(chars);
    StringObj *underlying = (StringObj *)unwrapObject(string);

    push(vm, string);

    vm->frames[vm->fp].ip = vm->frames[vm->fp].ip + underlying->length + 2;
}

static void interpretPrint(VirtualMachine *vm)
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
        StringObj *unwrapped = (StringObj *)unwrapObject(expression);
        int length = snprintf(NULL, 0, "%s", unwrapped->chars);
        line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%s", unwrapped->chars);
    }
    else if (isNil(expression))
    {
        int length = snprintf(NULL, 0, "%s", "nil");
        line = malloc(sizeof(char) * length + 1);
        snprintf(line, length + 1, "%s", "nil");
        line[length] = '\0';
    }

    if (vm->onStdOut != NULL)
    {
        vm->onStdOut(line);
    }
    vm->frames[vm->fp].ip = vm->frames[vm->fp].ip + 1;
}

static void interpretVarDecl(VirtualMachine *vm)
{
    CallFrame *currentFrame = getCurrentFrame(vm);
    push(vm, nil());
    currentFrame->ip = currentFrame->ip + getByteLengthFor(OP_VAR_DECL);
}

static void interpretVarAssign(VirtualMachine *vm)
{
    CallFrame *currentFrame = getCurrentFrame(vm);

    uint8_t offset = *(currentFrame->ip + 1);
    currentFrame->sp[offset] = pop(vm);

    currentFrame->ip = currentFrame->ip + 2;
}

static void interpretVarExpression(VirtualMachine *vm)
{
    CallFrame *currentFrame = getCurrentFrame(vm);

    uint8_t offset = *(currentFrame->ip + 1);
    Value value = currentFrame->sp[offset];
    push(vm, value);

    currentFrame->ip = currentFrame->ip + 2;
}

static void interpretGlobalVarDecl(VirtualMachine *vm)
{
    int constantIndex = *(vm->frames[vm->fp].ip + 1);
    Value constant = getCurrentFrameConstantAt(vm, constantIndex);
    StringObj *asString = (StringObj *)unwrapObject(constant);
    hashMapPut(&vm->global, asString, nil());

    vm->frames[vm->fp].ip = vm->frames[vm->fp].ip + 2;
}

static void interpretGlobalVarAssign(VirtualMachine *vm)
{
    int constantIndex = *(vm->frames[vm->fp].ip + 1);
    Value constant = getCurrentFrameConstantAt(vm, constantIndex);
    StringObj *asString = (StringObj *)unwrapObject(constant);

    hashMapPut(&vm->global, asString, pop(vm));
    vm->frames[vm->fp].ip = vm->frames[vm->fp].ip + 2;
}

static void interpretGlobalExpression(VirtualMachine *vm)
{
    int constantIndex = *(vm->frames[vm->fp].ip + 1);
    Value constant = getCurrentFrameConstantAt(vm, constantIndex);
    StringObj *asString = (StringObj *)unwrapObject(constant);

    push(vm, hashMapGet(&vm->global, asString));

    vm->frames[vm->fp].ip = vm->frames[vm->fp].ip + 2;
}

static void interpretPop(VirtualMachine *vm)
{
    // This guy is going to stay the same.
    pop(vm);
    vm->frames[vm->fp].ip = vm->frames[vm->fp].ip + 1;
}

static void interpretJumpIfFalse(VirtualMachine *vm)
{
    Value expression = pop(vm);
    bool unwrapped = unwrapBool(expression);

    if (!unwrapped)
    {
        uint8_t msb = *(vm->frames[vm->fp].ip + 1);
        uint8_t lsb = *(vm->frames[vm->fp].ip + 2);
        uint16_t jumpLocation = (msb << 8) | lsb;
        vm->frames[vm->fp].ip = &vm->frames[vm->fp].closure->function->bytecode->code[jumpLocation];
    }
    else
    {
        vm->frames[vm->fp].ip = vm->frames[vm->fp].ip + 3;
    }
}

static void interpretLoop(VirtualMachine *vm)
{
    uint8_t jumpOffset = *(vm->frames[vm->fp].ip + 1);
    vm->frames[vm->fp].ip = vm->frames[vm->fp].ip - (jumpOffset);
}

static void interpretJump(VirtualMachine *vm)
{
    uint8_t msb = *(vm->frames[vm->fp].ip + 1);
    uint8_t lsb = *(vm->frames[vm->fp].ip + 2);
    uint16_t jumpLocation = (msb << 8) | lsb;

    vm->frames[vm->fp].ip = &vm->frames[vm->fp].closure->function->bytecode->code[jumpLocation];
}

static void stdSysOut(char *message)
{
    printf(message);
    free(message);
}

CallFrame *prepareForCall(VirtualMachine *vm, FunctionObj *functionObj)
{
    vm->fp++;
    CallFrame *newFrame = getCurrentFrame(vm);

    ClosureObj *closure = malloc(sizeof(ClosureObj));
    initClosureObj(closure);
    closure->function = functionObj;

    newFrame->closure = closure;
    newFrame->ip = closure->function->bytecode->code;
    newFrame->currentStackIndex = 0;
    newFrame->sp = &vm->stack[0];

    if (vm->debugMode)
    {
        disassembleChunk(functionObj->bytecode, functionObj->name->chars, stdSysOut);
    }

    return newFrame;
}

static void interpretCall(VirtualMachine *vm)
{
    uint8_t *ip = getCurrentIp(vm);
    uint8_t argumentCount = *(ip + 1);

    CallFrame *currentFrame = getCurrentFrame(vm);
    currentFrame->ip = currentFrame->ip + 2;
    Value *startOfFunctionCall = &currentFrame->sp[currentFrame->currentStackIndex - argumentCount - 1];

    ClosureObj *toRun = unwrapClosureObj(*startOfFunctionCall);

    CallFrame *nextFrame = getNextFrame(vm);
    nextFrame->currentStackIndex = argumentCount;
    nextFrame->closure = toRun;
    nextFrame->sp = startOfFunctionCall + 1;
    nextFrame->ip = toRun->function->bytecode->code;

    if (vm->debugMode)
    {
        disassembleChunk(toRun->function->bytecode, toRun->function->name->chars, stdSysOut);
    }

    vm->fp++;
}

static bool hasReturnValue(CallFrame *frame, uint8_t numLocalArgs)
{
    int numFunctionArgs = frame->closure->function->arity;
    return frame->currentStackIndex == numFunctionArgs + numLocalArgs + 1;
}

static void interpretCloseUpvalue(VirtualMachine *vm)
{
    CallFrame *currentFrame = getCurrentFrame(vm);

    Value *popped = &currentFrame->sp[currentFrame->currentStackIndex - 1];
    currentFrame->currentStackIndex--;

    UpvalueObj *iterator = vm->upvalueHead;
    while (iterator != NULL) 
    {
        if (popped == iterator->location)
        {
            iterator->closed = *popped; 
            iterator->location = &iterator->closed;
            break;
        }

        iterator = iterator->next;
    }

    currentFrame->ip = currentFrame->ip + 1;
}

static void interpretReturn(VirtualMachine *vm)
{
    CallFrame *currentFrame = getCurrentFrame(vm);
    Value returnValue = nil();

    int numLocalArgs = *(currentFrame->ip + 1);
    if (hasReturnValue(currentFrame, numLocalArgs))
    {
        returnValue = pop(vm);
    }

    for (int i = 2; i < numLocalArgs + 2; i++)
    {
        OpCode popType = *(currentFrame->ip + i);
        if (popType == OP_POP)
        {
            interpretPop(vm);
        }
        else if (popType == OP_CLOSE_UPVALUE)
        {
            interpretCloseUpvalue(vm);
        } 
        else 
        {
            printf("Wrong thing was in after return function");
        }
    }

    vm->fp--;
    CallFrame *prevFrame = getCurrentFrame(vm);

    int offset = currentFrame->closure->function->arity + 1;

    prevFrame->currentStackIndex = prevFrame->currentStackIndex - offset;
    push(vm, returnValue);

    currentFrame->closure = NULL;
    currentFrame->currentStackIndex = 0;
    currentFrame->ip = NULL;
    currentFrame->sp = NULL;
}

static void interpretOr(VirtualMachine *vm)
{
    CallFrame *currentFrame = getCurrentFrame(vm);

    Value right = pop(vm);
    Value left = pop(vm);

    bool rightBool = unwrapBool(right);
    bool leftBool = unwrapBool(left);

    bool resultBool = leftBool || rightBool;
    Value result = wrapBool(resultBool);
    push(vm, result);

    currentFrame->ip = currentFrame->ip + 1;
}

static void interpretLessThanOrEquals(VirtualMachine *vm)
{
    CallFrame *currentFrame = getCurrentFrame(vm);

    Value right = pop(vm);
    Value left = pop(vm);

    double rightNum = unwrapNumber(right);
    double leftNum = unwrapNumber(left);

    Value result = wrapBool(leftNum <= rightNum);
    push(vm, result);

    currentFrame->ip = currentFrame->ip + 1;
}

static UpvalueObj *resolveUpvalue(VirtualMachine *vm, Upvalue staticUpvalue)
{
    CallFrame *currentFrame = getCurrentFrame(vm);

    UpvalueObj *upvalue = malloc(sizeof(UpvalueObj));
    upvalue->location = &currentFrame->sp[staticUpvalue.index];

    vm->upvalueHead = upvalue;
    return vm->upvalueHead;
}

static void interpretClosure(VirtualMachine *vm)
{
    CallFrame *currentFrame = getCurrentFrame(vm);

    Value function = pop(vm);
    ClosureObj *closure = malloc(sizeof(ClosureObj));

    initClosureObj(closure);
    closure->function = unwrapFunctionObj(function);
    closure->upvalues = malloc(sizeof(Upvalue *) * closure->function->numUpvalues);

    for (int i = 0; i < closure->function->numUpvalues; i++)
    {
        Upvalue staticUpvalue = closure->function->upvalues[i];
        closure->upvalues[staticUpvalue.index] = resolveUpvalue(vm, staticUpvalue);
    }

    Value result = wrapObject((Obj *)closure);
    push(vm, result);

    currentFrame->ip = currentFrame->ip + 1;
}

static void interpretGetUpvalue(VirtualMachine *vm)
{
    CallFrame *currentFrame = getCurrentFrame(vm);

    // bool isLocal = *(currentFrame->ip + 1);
    uint8_t index = *(currentFrame->ip + 2);
    UpvalueObj *upvalue = currentFrame->closure->upvalues[index];

    push(vm, (*upvalue->location));

    currentFrame->ip = currentFrame->ip + 3;
}



static bool isAtEndOfBytecode(VirtualMachine *vm)
{
    OpCode opCode = *(vm->frames[vm->fp].ip);
    return OP_RETURN == opCode && vm->fp == 0;
}

void interpret(VirtualMachine *vm)
{
    while (!isAtEndOfBytecode(vm))
    {
        OpCode opCode = *(vm->frames[vm->fp].ip);
        switch (opCode)
        {
        case OP_CONSTANT:
            interpretConstant(vm);
            break;
        case OP_NEGATE:
            interpretNegate(vm);
            break;
        case OP_ADD:
            interpretAdd(vm);
            break;
        case OP_MULT:
            interpretMultiplication(vm);
            break;
        case OP_DIV:
            interpretDivision(vm);
            break;
        case OP_SUB:
            interpretSubtraction(vm);
            break;
        case OP_LESS_THAN:
            interpretLessThan(vm);
            break;
        case OP_TRUE:
            interpretTrue(vm);
            break;
        case OP_FALSE:
            interpretFalse(vm);
            break;
        case OP_EQUAL:
            interpretEquals(vm);
            break;
        case OP_STRING:
            interpretString(vm);
            break;
        case OP_PRINT:
            interpretPrint(vm);
            break;
        case OP_VAR_DECL:
            interpretVarDecl(vm);
            break;
        case OP_VAR_ASSIGN:
            interpretVarAssign(vm);
            break;
        case OP_VAR_EXPRESSION:
            interpretVarExpression(vm);
            break;
        case OP_VAR_GLOBAL_DECL:
            interpretGlobalVarDecl(vm);
            break;
        case OP_VAR_GLOBAL_ASSIGN:
            interpretGlobalVarAssign(vm);
            break;
        case OP_VAR_GLOBAL_EXPRESSION:
            interpretGlobalExpression(vm);
            break;
        case OP_POP:
            interpretPop(vm);
            break;
        case OP_JUMP_IF_FALSE:
            interpretJumpIfFalse(vm);
            break;
        case OP_LOOP:
            interpretLoop(vm);
            break;
        case OP_JUMP:
            interpretJump(vm);
            break;
        case OP_CALL:
            interpretCall(vm);
            break;
        case OP_RETURN:
            interpretReturn(vm);
            break;
        case OP_OR:
            interpretOr(vm);
            break;
        case OP_LESS_THAN_EQUALS:
            interpretLessThanOrEquals(vm);
            break;
        case OP_CLOSURE:
            interpretClosure(vm);
            break;
        case OP_GET_UPVALUE:
            interpretGetUpvalue(vm);
            break;
        case OP_CLOSE_UPVALUE:
            interpretCloseUpvalue(vm);
            break;
        default:
            printf("Invalid op code.");
            break;
        }
    }
    if (isAtEndOfBytecode(vm))
    {
        vm->frames[vm->fp].ip = vm->frames[vm->fp].ip + 1;
    }
}

Value peek(VirtualMachine *vm)
{
    CallFrame *currentFrame = getCurrentFrame(vm);
    int stackPeekIndex = currentFrame->currentStackIndex - 1;
    Value popped = currentFrame->sp[stackPeekIndex];
    return popped;
}

void push(VirtualMachine *vm, Value value)
{
    CallFrame *currentFrame = getCurrentFrame(vm);
    currentFrame->sp[currentFrame->currentStackIndex] = value;

    currentFrame->currentStackIndex++;
}

Value pop(VirtualMachine *vm)
{
    CallFrame *currentFrame = getCurrentFrame(vm);
    int stackPeekIndex = currentFrame->currentStackIndex - 1;
    Value popped = currentFrame->sp[stackPeekIndex];
    currentFrame->currentStackIndex--;
    return popped;
}