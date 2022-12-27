#include "compiler.h"
#include "chunk.h"
#include "scanner.h"
#include "vm.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "functionobj.h"
#include <stdio.h>
#include "disassembler.h"

typedef struct VariableBindingStackLocation
{
    Token token;
} VariableBindingStackLocation;

typedef struct FunctionCompiler
{
    FunctionObj *compiling;
    HashMap functions;
    int stackDepth;
    int blockDepth;
    VariableBindingStackLocation stack[5];
} FunctionCompiler;

typedef struct Parser
{
    TokenArrayIterator *tokens;
    FunctionCompiler compilers[256];
    int depth;
} Parser;

typedef void (*ParseFn)(Parser *);

typedef struct ParseRule
{
    ParseFn prefix;
    ParseFn infix;
    Precedence Precedence;
} ParseRule;

static FunctionCompiler *getCurrentCompiler(Parser *parser)
{
    FunctionCompiler *current = &parser->compilers[parser->depth];
    return current;
}

static Chunk *getCurrentCompilerBytecode(Parser *parser)
{
    return getCurrentCompiler(parser)->compiling->bytecode;
}

static HashMap *getCurrentCompilerFunctions(Parser *parser)
{
    return &getCurrentCompiler(parser)->functions;
}

static void advanceCompilerFunction(Parser *parser, FunctionObj *function)
{
    parser->depth++;
    FunctionCompiler *current = getCurrentCompiler(parser);
    current->compiling = function;
    current->blockDepth = 0;
    current->stackDepth = 0;
}

static void undoCompilerFunction(Parser *parser)
{
    FunctionCompiler *current = getCurrentCompiler(parser);
    current->compiling = NULL;
    parser->depth--;
}

static bool isAtTopLevel(Parser *parser);
static bool isInMainFunction(Parser *parser);
static void number(Parser *);
static void binary(Parser *);
static void unary(Parser *);
static void literal(Parser *);
static void identifier(Parser *parser);
static void callable(Parser *parser);
static void ifStatement(Parser *);
static void expression(Parser *);
static void parseExpression(Precedence, Parser *);
static bool isAtEndOfStatement(Parser *);
static bool isAtEndOfConditional(Parser *parser);
static bool isGlobalBinding(Parser *, Token);
static uint8_t getVariableBinding(Parser *, Token);
static int getVariableBindingAsInt(Parser *parser, Token token);
static void statement(Parser *);

ParseRule rules[] = {
    [TOKEN_NUMBER] = {number, NULL, PREC_NONE},
    [TOKEN_PLUS] = {NULL, binary, PREC_TERM},
    [TOKEN_MINUS] = {unary, binary, PREC_UNARY},
    [TOKEN_TRUE] = {literal, NULL, PREC_NONE},
    [TOKEN_LESS] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_LESS_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_OR] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_BANG_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TOKEN_FALSE] = {literal, NULL, PREC_NONE},
    [TOKEN_STAR] = {NULL, binary, PREC_FACTOR},
    [TOKEN_STRING] = {literal, NULL, PREC_FACTOR},
    [TOKEN_IDENTIFIER] = {identifier, NULL, PREC_CALL},
    [TOKEN_LEFT_PAREN] = {NULL, callable, PREC_CALL},
    [TOKEN_SLASH] = {NULL, binary, PREC_FACTOR}};

static ParseRule *getRule(TokenType tokenType)
{
    return &rules[tokenType];
}

static void number(Parser *parser)
{
    Token shouldBeNumber = popToken(parser->tokens);

    double number = strtod(shouldBeNumber.lexeme, NULL);
    Value value = wrapNumber(number);

    int valueConstantIndex = addConstant(getCurrentCompilerBytecode(parser), value);
    writeChunk(getCurrentCompilerBytecode(parser), OP_CONSTANT);
    writeChunk(getCurrentCompilerBytecode(parser), valueConstantIndex);
}

static void literal(Parser *parser)
{
    Token shouldBeTrue = popToken(parser->tokens);

    if (shouldBeTrue.type == TOKEN_TRUE)
    {
        writeChunk(getCurrentCompilerBytecode(parser), OP_TRUE);
    }
    else if (shouldBeTrue.type == TOKEN_FALSE)
    {
        writeChunk(getCurrentCompilerBytecode(parser), OP_FALSE);
    }
    else if (shouldBeTrue.type == TOKEN_STRING)
    {
        writeChunk(getCurrentCompilerBytecode(parser), OP_STRING);
        writeString(getCurrentCompilerBytecode(parser), (const char *)shouldBeTrue.lexeme);
    }
}

// Checking if something is a function might be the wrong bet?
// So in the foo case that I am currently looking at,
// we could go up the function tree and find a function at RUNTIME?
// What would that mean I would have to do?
// Fuck that has to be sooo slow though?
// So the problem is is that my function is stored in the following manner:

// Value of 2 as number, is an index into the constant array of the current function.
// If you were to return this value of 2 to someone else, it would not make any sense.
// So what do we need to do now?

// So when you run into an identifier DURING RUNTIME maybe you should check if the given
// thing is a function or not, then run accordingly.

// OR better yet, when someone is to receive a value it is up to them to call it properly.
// If it is not callable, you are out of luck.

// How here's the thing...
// Is there an  efficient way of figuring out where a  fuction lives without having to go through maps?
// Its odd that I use a constant... it really should be through a identifier right?
// It could be w/e is on the stack at the moment?
// You are just going to call w/e is on the bottom of the call stack?
//
// When you define a function, it can do the following things:
// 1) The block is the enclosing scope of where a function is defined.
// A function is scoped within the block in which its defined?
// If you are trying to call something, what happens?
// The value must be of a function obj.
// Thus, the value must be OBJECT value whose pointer is going to FunctionObj.
// How does this work on the identifier.
// So when are you compiling something...
// Wait if we keep the function within the constant.
// Have a map from name to constant value.
// So if you are trying to figure

static bool isFunction(Parser *parser, const char *functionName)
{
    StringObj *functionNameAsString = asString(functionName);

    for (int i = parser->depth; i >= 0; i--)
    {
        FunctionCompiler *compiler = &parser->compilers[i];
        Value functionIfExists = hashMapGet(&compiler->functions, functionNameAsString);

        if (!isNil(functionIfExists))
        {
            freeStringObj(functionNameAsString);
            return true;
        }
    }

    freeStringObj(functionNameAsString);
    return false;
}

static FunctionObj *getFunctionObj(Parser *parser, const char *functionName)
{
    StringObj *functionNameAsString = asString(functionName);

    for (int i = parser->depth; i >= 0; i--)
    {
        FunctionCompiler *compiler = &parser->compilers[i];
        Value functionIfExists = hashMapGet(&compiler->functions, functionNameAsString);

        if (!isNil(functionIfExists))
        {
            // So at this moment... there is a constant in the bytecode that has our function in it...
            int functionConstantIndex = (int)unwrapNumber(functionIfExists);

            freeStringObj(functionNameAsString);
            return (FunctionObj *)unwrapObject(getConstantAt(compiler->compiling->bytecode, functionConstantIndex));
        }
    }

    freeStringObj(functionNameAsString);
    return NULL;
}

static bool isLocallyDefinedFunction(Parser *parser, const char *functionName)
{
    FunctionCompiler *compiler = getCurrentCompiler(parser);

    StringObj *functionNameAsString = asString(functionName);
    Value functionIfExists = hashMapGet(&compiler->functions, functionNameAsString);
    freeStringObj(functionNameAsString);
    return !isNil(functionIfExists);
}

static uint8_t getLocalFunctionConstantLocation(Parser *parser, const char *functionName)
{
    FunctionCompiler *compiler = getCurrentCompiler(parser);

    StringObj *functionNameAsString = asString(functionName);
    Value functionIfExists = hashMapGet(&compiler->functions, functionNameAsString);
    freeStringObj(functionNameAsString);
    return unwrapNumber(functionIfExists);
}

static void callable(Parser *parser)
{
    popToken(parser->tokens);
    uint8_t functionArgumentCount = 0;

    if (peekAtToken(parser->tokens).type != TOKEN_RIGHT_PAREN)
    {
        do
        {
            expression(parser);
            functionArgumentCount++;
        } while (popToken(parser->tokens).type == TOKEN_COMMA);
    }
    else
    {
        popToken(parser->tokens);
    }

    writeChunk(getCurrentCompilerBytecode(parser), OP_CALL);
    writeChunk(getCurrentCompilerBytecode(parser), functionArgumentCount);
}

static void identifier(Parser *parser)
{
    Token shouldBeId = popToken(parser->tokens);

    if (shouldBeId.type == TOKEN_IDENTIFIER)
    {
        if (isFunction(parser, shouldBeId.lexeme))
        {
            if (isLocallyDefinedFunction(parser, shouldBeId.lexeme))
            {
                int constantLocation = getLocalFunctionConstantLocation(parser, shouldBeId.lexeme);
                writeChunk(getCurrentCompilerBytecode(parser), OP_CONSTANT);
                writeChunk(getCurrentCompilerBytecode(parser), constantLocation);
            }
            else
            {
                FunctionCompiler *currentCompiler = getCurrentCompiler(parser);
                FunctionObj *functionObj = getFunctionObj(parser, shouldBeId.lexeme);
                int constantIndex = addConstant(currentCompiler->compiling->bytecode, wrapObject((Obj *)functionObj));
                hashMapPut(&currentCompiler->functions, functionObj->name, wrapNumber(constantIndex));

                writeChunk(currentCompiler->compiling->bytecode, OP_CONSTANT);
                writeChunk(currentCompiler->compiling->bytecode, constantIndex);
            }
        }
        else if (isGlobalBinding(parser, shouldBeId))
        {
            FunctionCompiler *currentCompiler = getCurrentCompiler(parser);
            Token maybeEquals = peekAtToken(parser->tokens);

            if (maybeEquals.type == TOKEN_EQUAL)
            {
                popToken(parser->tokens);
                expression(parser);

                writeChunk(currentCompiler->compiling->bytecode, OP_VAR_GLOBAL_ASSIGN);
                int constantLocation = addConstant(currentCompiler->compiling->bytecode, wrapString(shouldBeId.lexeme));
                writeChunk(currentCompiler->compiling->bytecode, constantLocation);
            }
            else
            {
                int constantLocation = addConstant(currentCompiler->compiling->bytecode, wrapString(shouldBeId.lexeme));
                writeChunk(currentCompiler->compiling->bytecode, OP_VAR_GLOBAL_EXPRESSION);
                writeChunk(currentCompiler->compiling->bytecode, constantLocation);
            }
        }
        else
        {
            FunctionCompiler *currentCompiler = getCurrentCompiler(parser);
            Token maybeEquals = peekAtToken(parser->tokens);
            if (maybeEquals.type == TOKEN_EQUAL)
            {

                popToken(parser->tokens);
                expression(parser);

                writeChunk(currentCompiler->compiling->bytecode, OP_VAR_ASSIGN);
                uint8_t offset = getVariableBinding(parser, shouldBeId);
                writeChunk(currentCompiler->compiling->bytecode, offset);
            }
            else
            {
                uint8_t stackLocation = getVariableBinding(parser, shouldBeId);

                writeChunk(currentCompiler->compiling->bytecode, OP_VAR_EXPRESSION);
                writeChunk(currentCompiler->compiling->bytecode, stackLocation);
            }
        }
    }
}

static void binary(Parser *parser)
{
    Token operator= popToken(parser->tokens);
    ParseRule *parseRule = getRule(operator.type);

    parseExpression(parseRule->Precedence + 1, parser);

    if (operator.type == TOKEN_PLUS)
    {
        writeChunk(getCurrentCompilerBytecode(parser), OP_ADD);
    }
    else if (operator.type == TOKEN_STAR)
    {
        writeChunk(getCurrentCompilerBytecode(parser), OP_MULT);
    }
    else if (operator.type == TOKEN_MINUS)
    {
        writeChunk(getCurrentCompilerBytecode(parser), OP_SUB);
    }
    else if (operator.type == TOKEN_SLASH)
    {
        writeChunk(getCurrentCompilerBytecode(parser), OP_DIV);
    }
    else if (operator.type == TOKEN_LESS)
    {
        writeChunk(getCurrentCompilerBytecode(parser), OP_LESS_THAN);
    }
    else if (operator.type == TOKEN_OR)
    {
        writeChunk(getCurrentCompilerBytecode(parser), OP_OR);
    }
    else if (operator.type == TOKEN_LESS_EQUAL)
    {
        writeChunk(getCurrentCompilerBytecode(parser), OP_LESS_THAN_EQUALS);
    }
    else if (operator.type == TOKEN_BANG_EQUAL)
    {
        writeChunk(getCurrentCompilerBytecode(parser), OP_EQUAL);
        writeChunk(getCurrentCompilerBytecode(parser), OP_NEGATE);
    }
}

static void unary(Parser *parser)
{
    Token operator= popToken(parser->tokens);
    ParseRule *parseRule = getRule(operator.type);

    parseExpression(parseRule->Precedence + 1, parser);

    writeChunk(getCurrentCompilerBytecode(parser), OP_NEGATE);
}

void initInterpreter(Interpreter *interpreter)
{
    initVirtualMachine(&interpreter->vm);
    interpreter->onStdOut = NULL;
    interpreter->debugMode = false;
}

void freeInterpreter(Interpreter *interpreter)
{
}

void initParser(Parser *parser)
{
    parser->tokens = NULL;
    parser->depth = -1;

    initHashMap(getCurrentCompilerFunctions(parser));
}

void freeParser(Parser *parser)
{
    freeHashMap(getCurrentCompilerFunctions(parser));
}

void compile(FunctionObj *functionObj, TokenArrayIterator *tokens)
{
    Parser parser;
    initParser(&parser);
    parser.tokens = tokens;

    advanceCompilerFunction(&parser, functionObj);

    while (hasNextToken(parser.tokens))
    {
        statement(&parser);
    }

    undoCompilerFunction(&parser);

    writeChunk(functionObj->bytecode, OP_RETURN);

    // freeParser(&parser);
}

TokenArrayIterator tokenize(const char *sourceCode)
{
    TokenArray tokens = parseTokens(sourceCode);
    TokenArrayIterator iterator = tokensIterator(tokens);
    return iterator;
}

void runInterpreter(Interpreter *interpreter, const char *sourceCode)
{
    FunctionObj functionObj;
    initFunctionObj(&functionObj);

    TokenArrayIterator tokens = tokenize(sourceCode);
    compile(&functionObj, &tokens);

    interpreter->vm.onStdOut = interpreter->onStdOut;
    interpreter->vm.debugMode = interpreter->debugMode;
    prepareForCall(&interpreter->vm, &functionObj);
    interpret(&interpreter->vm);

    freeFunctionObj(&functionObj);
}

static void printStatement(Parser *parser)
{
    popToken(parser->tokens);
    expression(parser);
    writeChunk(getCurrentCompilerBytecode(parser), OP_PRINT);
    popToken(parser->tokens);
}

static bool isInMainFunction(Parser *parser)
{
    return parser->depth == -1;
}

static bool isAtTopLevel(Parser *parser)
{
    FunctionCompiler *compiler = getCurrentCompiler(parser);
    return compiler->blockDepth == 0;
}

static void defineVariableBinding(Parser *parser, Token token)
{
    bool shouldDefineVariableBinding = true;
    if (isInMainFunction(parser))
    {
        if (isAtTopLevel(parser))
        {
            shouldDefineVariableBinding = false;
        }
    }

    if (shouldDefineVariableBinding)
    {
        FunctionCompiler *compiler = getCurrentCompiler(parser);
        VariableBindingStackLocation *slot = &compiler->stack[compiler->stackDepth];
        slot->token = token;

        compiler->stackDepth++;
    }
}

#define _BINDING_NOT_FOUND_ -1
static bool isGlobalBinding(Parser *parser, Token token)
{
    int bindingLocation = getVariableBindingAsInt(parser, token);
    return bindingLocation == _BINDING_NOT_FOUND_;
}

static uint8_t getVariableBinding(Parser *parser, Token token)
{
    return getVariableBindingAsInt(parser, token);
}

static int getVariableBindingAsInt(Parser *parser, Token token)
{
    int bindingLocation = _BINDING_NOT_FOUND_;
    FunctionCompiler *compiler = getCurrentCompiler(parser);
    for (int i = compiler->stackDepth - 1; i >= 0; i--)
    {
        VariableBindingStackLocation *slot = &compiler->stack[i];
        if (!strcmp(token.lexeme, slot->token.lexeme))
        {
            bindingLocation = i;
            break;
        }
    }
    return bindingLocation;
}

static bool isInGlobalScope(Parser *parser)
{
    return parser->depth == -1;
}

static void variableDecl(Parser *parser)
{
    popToken(parser->tokens);
    Token identifier = popToken(parser->tokens);

    if (isInGlobalScope(parser))
    {
        writeChunk(getCurrentCompilerBytecode(parser), OP_VAR_GLOBAL_DECL);
        Value asString = wrapString(identifier.lexeme);
        int constantLocation = addConstant(getCurrentCompilerBytecode(parser), asString);
        writeChunk(getCurrentCompilerBytecode(parser), constantLocation);
    }
    else
    {
        writeChunk(getCurrentCompilerBytecode(parser), OP_VAR_DECL);
        defineVariableBinding(parser, identifier);
    }

    Token equalsOrSemicolon = popToken(parser->tokens);

    if (equalsOrSemicolon.type == TOKEN_EQUAL)
    {
        expression(parser);
        if (isGlobalBinding(parser, identifier))
        {
            writeChunk(getCurrentCompilerBytecode(parser), OP_VAR_GLOBAL_ASSIGN);
            Value asString = wrapString(identifier.lexeme);
            int constantLocation = addConstant(getCurrentCompilerBytecode(parser), asString);
            writeChunk(getCurrentCompilerBytecode(parser), constantLocation);
        }
        else
        {
            writeChunk(getCurrentCompilerBytecode(parser), OP_VAR_ASSIGN);
            uint8_t offset = getVariableBinding(parser, identifier);
            writeChunk(getCurrentCompilerBytecode(parser), offset);
        }
        popToken(parser->tokens);
    }
}

static void expressionStatement(Parser *parser)
{
    expression(parser);
    writeChunk(getCurrentCompilerBytecode(parser), OP_POP);
    popToken(parser->tokens);
}

static void blockStart(Parser *parser)
{
    FunctionCompiler *current = getCurrentCompiler(parser);
    current->blockDepth++;
}

static void blockEnd(Parser *parser, int stackDepthStart)
{
    FunctionCompiler *current = getCurrentCompiler(parser);
    current->blockDepth--;
    current->stackDepth = stackDepthStart;
}

static void releaseLocals(Parser *parser, int numLocals)
{
    for (int i = 0; i < numLocals; i++)
    {
        writeChunk(getCurrentCompilerBytecode(parser), OP_POP);
    }
}

static void blockStatement(Parser *parser)
{
    FunctionCompiler *current = getCurrentCompiler(parser);
    // This does not look semantically correct.
    popToken(parser->tokens);

    blockStart(parser);

    int stackDepthStart = current->stackDepth;
    while (peekAtToken(parser->tokens).type != TOKEN_RIGHT_BRACE)
    {
        statement(parser);
    }

    int numLocals = current->stackDepth - stackDepthStart;
    releaseLocals(parser, numLocals);

    blockEnd(parser, stackDepthStart);
    popToken(parser->tokens);
}

static void ifStatement(Parser *parser)
{
    popToken(parser->tokens);
    popToken(parser->tokens);

    expression(parser);

    writeChunk(getCurrentCompilerBytecode(parser), OP_JUMP_IF_FALSE);
    int currentLocation = getCurrentCompilerBytecode(parser)->count;
    writeShort(getCurrentCompilerBytecode(parser), UINT16_MAX);

    popToken(parser->tokens);
    blockStatement(parser);

    overwriteShort(getCurrentCompilerBytecode(parser), currentLocation, getCurrentCompilerBytecode(parser)->count);
}

static void whileStatement(Parser *parser)
{
    popToken(parser->tokens); // popping while
    popToken(parser->tokens); // popping (

    int startOfWhileExpression = getCurrentCompilerBytecode(parser)->count + 1;
    expression(parser);

    popToken(parser->tokens); // popping )

    writeChunk(getCurrentCompilerBytecode(parser), OP_JUMP_IF_FALSE);
    int jumpIfFalseLocation = getCurrentCompilerBytecode(parser)->count;
    writeShort(getCurrentCompilerBytecode(parser), UINT16_MAX);

    blockStatement(parser);

    int blockLength = getCurrentCompilerBytecode(parser)->count - startOfWhileExpression + 1;

    writeChunk(getCurrentCompilerBytecode(parser), OP_LOOP);
    writeChunk(getCurrentCompilerBytecode(parser), blockLength);

    overwriteShort(getCurrentCompilerBytecode(parser), jumpIfFalseLocation, getCurrentCompilerBytecode(parser)->count);
}

static void compileInitStatements(Parser *parser)
{
    Token varDeclOrSemicolon = peekAtToken(parser->tokens);
    // Can either be var or semicolon
    if (varDeclOrSemicolon.type == TOKEN_VAR)
    {
        variableDecl(parser);
    }
    else
    {
        popToken(parser->tokens);
    }
}

static int compileExpressionStatement(Parser *parser)
{
    int startOfIterablePortion = getCurrentCompilerBytecode(parser)->count + 1;
    expression(parser);

    popToken(parser->tokens); // ; that ends the expression?

    return startOfIterablePortion;
}

static int writeEmptyIfFalseJumpStatement(Parser *parser)
{
    writeChunk(getCurrentCompilerBytecode(parser), OP_JUMP_IF_FALSE);
    int jumpIfFalseLocation = getCurrentCompilerBytecode(parser)->count;
    writeShort(getCurrentCompilerBytecode(parser), UINT16_MAX);

    return jumpIfFalseLocation;
}

static int writeEmptyJumpStatement(Parser *parser)
{
    writeChunk(getCurrentCompilerBytecode(parser), OP_JUMP);
    int forLoopUpdateLocation = getCurrentCompilerBytecode(parser)->count;
    writeShort(getCurrentCompilerBytecode(parser), UINT16_MAX);
    return forLoopUpdateLocation;
}

static int compileUpdateStatements(Parser *parser, int startOfExpressionLocation)
{
    int startOfCompileUpdateStatements = getCurrentCompilerBytecode(parser)->count;
    Token varAssignOrSemicolon = peekAtToken(parser->tokens);
    if (varAssignOrSemicolon.type == TOKEN_IDENTIFIER)
    {
        expression(parser);
        popToken(parser->tokens);
    }
    else
    {
        popToken(parser->tokens);
    }

    int jumpOffset = getCurrentCompilerBytecode(parser)->count - startOfExpressionLocation + 1;
    writeChunk(getCurrentCompilerBytecode(parser), OP_LOOP);
    writeChunk(getCurrentCompilerBytecode(parser), jumpOffset);

    return startOfCompileUpdateStatements;
}

static void forStatement(Parser *parser)
{
    blockStart(parser);
    int stackDepthStart = parser->depth;

    popToken(parser->tokens); // should have been for
    popToken(parser->tokens); // should have been (

    compileInitStatements(parser);

    int startOfIterablePortion = compileExpressionStatement(parser);
    int jumpIfFalseValueLocation = writeEmptyIfFalseJumpStatement(parser);
    int expressionToBodyJumpLocation = writeEmptyJumpStatement(parser);

    int startOfUpdateStatements = compileUpdateStatements(parser, startOfIterablePortion);

    overwriteShort(getCurrentCompilerBytecode(parser), expressionToBodyJumpLocation, getCurrentCompilerBytecode(parser)->count);

    popToken(parser->tokens); // should have been a )

    blockStatement(parser);

    int blockLength = getCurrentCompilerBytecode(parser)->count - startOfUpdateStatements;

    writeChunk(getCurrentCompilerBytecode(parser), OP_LOOP);
    writeChunk(getCurrentCompilerBytecode(parser), blockLength);

    blockEnd(parser, stackDepthStart);

    overwriteShort(getCurrentCompilerBytecode(parser), jumpIfFalseValueLocation, getCurrentCompilerBytecode(parser)->count);
}

static FunctionObj *defineNewLocalFunction(Parser *parser, Token funcId)
{
    FunctionObj *newFunctionDecl = malloc(sizeof(FunctionObj));
    initFunctionObj(newFunctionDecl);
    newFunctionDecl->name = asString(funcId.lexeme);

    int constantIndex = addConstant(getCurrentCompilerBytecode(parser), wrapObject((Obj *)newFunctionDecl));
    hashMapPut(getCurrentCompilerFunctions(parser), newFunctionDecl->name, wrapNumber(constantIndex));

    return newFunctionDecl;
}

static int funcDeclArgs(Parser *parser)
{
    int numFunctionArgs = 0;
    popToken(parser->tokens);
    Token maybeRightParen = peekAtToken(parser->tokens);

    if (maybeRightParen.type != TOKEN_RIGHT_PAREN)
    {
        while (true)
        {
            Token funcArg = popToken(parser->tokens);
            Token commaOrParen = popToken(parser->tokens);

            defineVariableBinding(parser, funcArg);
            numFunctionArgs++;

            if (commaOrParen.type == TOKEN_RIGHT_PAREN)
            {
                break;
            }
        }
    }
    else
    {
        popToken(parser->tokens);
    }
    return numFunctionArgs;
}

static void funcDecl(Parser *parser)
{
    popToken(parser->tokens);
    Token funcId = popToken(parser->tokens);

    FunctionObj *newLocalFunction = defineNewLocalFunction(parser, funcId);
    advanceCompilerFunction(parser, newLocalFunction);

    int numArgs = funcDeclArgs(parser);
    newLocalFunction->arity = numArgs;

    blockStatement(parser);
    writeChunk(getCurrentCompilerBytecode(parser), OP_RETURN);
    undoCompilerFunction(parser);
}

static void returnStmt(Parser *parser)
{
    popToken(parser->tokens);
    expression(parser);
    writeChunk(getCurrentCompilerBytecode(parser), OP_RETURN);
    popToken(parser->tokens);
}

static void statement(Parser *parser)
{
    Token peeked = peekAtToken(parser->tokens);

    if (peeked.type == TOKEN_PRINT)
    {
        printStatement(parser);
    }
    else if (peeked.type == TOKEN_VAR)
    {
        variableDecl(parser);
    }
    else if (peeked.type == TOKEN_IDENTIFIER)
    {
        expression(parser);
        popToken(parser->tokens);
    }
    else if (peeked.type == TOKEN_LEFT_BRACE)
    {
        blockStatement(parser);
    }
    else if (peeked.type == TOKEN_IF)
    {
        ifStatement(parser);
    }
    else if (peeked.type == TOKEN_WHILE)
    {
        whileStatement(parser);
    }
    else if (peeked.type == TOKEN_FOR)
    {
        forStatement(parser);
    }
    else if (peeked.type == TOKEN_FUN)
    {
        funcDecl(parser);
    }
    else if (peeked.type == TOKEN_RETURN)
    {
        returnStmt(parser);
    }
    else
    {
        expressionStatement(parser);
    }
}

static void expression(Parser *parser)
{
    parseExpression(PREC_ASSIGNMENT, parser);
}

static bool isAtEndOfExpression(Parser *);

static void parseExpression(Precedence precedence, Parser *parser)
{
    Token prefixToken = peekAtToken(parser->tokens);
    ParseFn prefixFunction = getRule(prefixToken.type)->prefix;

    prefixFunction(parser);

    Token maybeInfixToken = peekAtToken(parser->tokens);
    ParseRule *infixParseRule = getRule(maybeInfixToken.type);
    while (!isAtEndOfExpression(parser) && precedence <= infixParseRule->Precedence)
    {
        ParseFn infixFunction = getRule(maybeInfixToken.type)->infix;
        infixFunction(parser);
        maybeInfixToken = peekAtToken(parser->tokens);
    }
}

static bool isAtEndOfFunctionArgument(Parser *parser);
static bool isAtEndOfExpression(Parser *parser)
{
    return !hasNextToken(parser->tokens) || isAtEndOfStatement(parser) || isAtEndOfConditional(parser) || isAtEndOfFunctionArgument(parser);
}

static bool isAtEndOfStatement(Parser *parser)
{
    return peekAtToken(parser->tokens).type == TOKEN_SEMICOLON;
}

static bool isAtEndOfConditional(Parser *parser)
{
    return peekAtToken(parser->tokens).type == TOKEN_RIGHT_PAREN;
}

static bool isAtEndOfFunctionArgument(Parser *parser)
{
    return peekAtToken(parser->tokens).type == TOKEN_COMMA;
}