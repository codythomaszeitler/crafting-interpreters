#include "compiler.h"
#include "chunk.h"
#include "scanner.h"
#include "vm.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct LocationStackLocation
{
    Token token;
} LocationStackLocation;

typedef struct Parser
{
    TokenArrayIterator *tokens;
    Chunk *compiling;

    int stackDepth;
    int blockDepth;
    LocationStackLocation stack[256];
} Parser;

typedef void (*ParseFn)(Parser *);

typedef struct ParseRule
{
    ParseFn prefix;
    ParseFn infix;
    Precedence Precedence;
} ParseRule;

static void number(Parser *);
static void binary(Parser *);
static void unary(Parser *);
static void literal(Parser *);
static void ifStatement(Parser *);
static void expression(Parser *);
static void parseExpression(Precedence, Parser *);
static bool isAtEndOfStatement(Parser *);
static bool isGlobalBinding(Parser *, Token);
static uint8_t getVariableBinding(Parser *, Token);
static int getVariableBindingAsInt(Parser *parser, Token token);
static void statement(Parser *);

ParseRule rules[] = {
    [TOKEN_NUMBER] = {number, NULL, PREC_NONE},
    [TOKEN_PLUS] = {NULL, binary, PREC_TERM},
    [TOKEN_MINUS] = {unary, binary, PREC_UNARY},
    [TOKEN_TRUE] = {literal, NULL, PREC_NONE},
    [TOKEN_BANG_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TOKEN_FALSE] = {literal, NULL, PREC_NONE},
    [TOKEN_STAR] = {NULL, binary, PREC_FACTOR},
    [TOKEN_STRING] = {literal, NULL, PREC_FACTOR},
    [TOKEN_IDENTIFIER] = {literal, NULL, PREC_NONE},
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

    int valueConstantIndex = addConstant(parser->compiling, value);
    writeChunk(parser->compiling, OP_CONSTANT);
    writeChunk(parser->compiling, valueConstantIndex);
}

static void literal(Parser *parser)
{
    Token shouldBeTrue = popToken(parser->tokens);

    if (shouldBeTrue.type == TOKEN_TRUE)
    {
        writeChunk(parser->compiling, OP_TRUE);
    }
    else if (shouldBeTrue.type == TOKEN_FALSE)
    {
        writeChunk(parser->compiling, OP_FALSE);
    }
    else if (shouldBeTrue.type == TOKEN_STRING)
    {
        writeChunk(parser->compiling, OP_STRING);
        writeString(parser->compiling, (const char *)shouldBeTrue.lexeme);
    }
    else if (shouldBeTrue.type == TOKEN_IDENTIFIER)
    {
        if (isGlobalBinding(parser, shouldBeTrue))
        {
            int constantLocation = addConstant(parser->compiling, wrapString(shouldBeTrue.lexeme));
            writeChunk(parser->compiling, OP_VAR_GLOBAL_EXPRESSION);
            writeChunk(parser->compiling, constantLocation);
        }
        else
        {
            uint8_t stackLocation = getVariableBinding(parser, shouldBeTrue);

            writeChunk(parser->compiling, OP_VAR_EXPRESSION);
            writeChunk(parser->compiling, stackLocation);
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
        writeChunk(parser->compiling, OP_ADD);
    }
    else if (operator.type == TOKEN_STAR)
    {
        writeChunk(parser->compiling, OP_MULT);
    }
    else if (operator.type == TOKEN_MINUS)
    {
        writeChunk(parser->compiling, OP_SUB);
    }
    else if (operator.type == TOKEN_SLASH)
    {
        writeChunk(parser->compiling, OP_DIV);
    }
    else if (operator.type == TOKEN_BANG_EQUAL)
    {
        writeChunk(parser->compiling, OP_EQUAL);
        writeChunk(parser->compiling, OP_NEGATE);
    }
}

static void unary(Parser *parser)
{
    Token operator= popToken(parser->tokens);
    ParseRule *parseRule = getRule(operator.type);

    parseExpression(parseRule->Precedence + 1, parser);

    writeChunk(parser->compiling, OP_NEGATE);
}

void initInterpreter(Interpreter *interpreter)
{
    initVirtualMachine(&interpreter->vm);
}

void freeInterpreter(Interpreter *interpreter)
{
}

void runInterpreter(Interpreter *interpreter, const char *sourceCode)
{
    Chunk bytecode;
    initChunk(&bytecode);

    compile(&bytecode, sourceCode);

    interpreter->vm.onStdOut = interpreter->onStdOut;
    interpret(&interpreter->vm, &bytecode);
}

static void printStatement(Parser *parser)
{
    popToken(parser->tokens);
    expression(parser);
    writeChunk(parser->compiling, OP_PRINT);
    popToken(parser->tokens);
}

static void defineVariableBinding(Parser *parser, Token token)
{
    if (parser->blockDepth != -1)
    {
        LocationStackLocation *slot = &parser->stack[parser->stackDepth];
        slot->token = token;

        parser->stackDepth++;
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
    if (parser->blockDepth != -1)
    {
        for (int i = parser->stackDepth - 1; i >= 0; i--)
        {
            LocationStackLocation *slot = &parser->stack[i];
            if (!strcmp(token.lexeme, slot->token.lexeme))
            {
                bindingLocation = i;
                break;
            }
        }
    }
    return bindingLocation;
}

static bool isInGlobalScope(Parser *parser)
{
    return parser->blockDepth == -1;
}

static void variableDecl(Parser *parser)
{
    popToken(parser->tokens);
    Token identifier = popToken(parser->tokens);

    if (isInGlobalScope(parser))
    {
        writeChunk(parser->compiling, OP_VAR_GLOBAL_DECL);
        Value asString = wrapString(identifier.lexeme);
        int constantLocation = addConstant(parser->compiling, asString);
        writeChunk(parser->compiling, constantLocation);
    }
    else
    {
        writeChunk(parser->compiling, OP_VAR_DECL);
        defineVariableBinding(parser, identifier);
    }

    Token equalsOrSemicolon = popToken(parser->tokens);

    if (equalsOrSemicolon.type == TOKEN_EQUAL)
    {
        expression(parser);
        if (isGlobalBinding(parser, identifier))
        {
            writeChunk(parser->compiling, OP_VAR_GLOBAL_ASSIGN);
            Value asString = wrapString(identifier.lexeme);
            int constantLocation = addConstant(parser->compiling, asString);
            writeChunk(parser->compiling, constantLocation);
        }
        else
        {
            writeChunk(parser->compiling, OP_VAR_ASSIGN);
            uint8_t offset = getVariableBinding(parser, identifier);
            writeChunk(parser->compiling, offset);
        }
        popToken(parser->tokens);
    }
}

static void variableAssignment(Parser *parser)
{
    Token identifier = popToken(parser->tokens);
    popToken(parser->tokens);

    expression(parser);

    if (isGlobalBinding(parser, identifier))
    {
        writeChunk(parser->compiling, OP_VAR_GLOBAL_ASSIGN);
        int constantLocation = addConstant(parser->compiling, wrapString(identifier.lexeme));
        writeChunk(parser->compiling, constantLocation);
    }
    else
    {
        writeChunk(parser->compiling, OP_VAR_ASSIGN);
        uint8_t offset = getVariableBinding(parser, identifier);
        writeChunk(parser->compiling, offset);
    }

    popToken(parser->tokens);
}

static void blockStatement(Parser *parser)
{
    popToken(parser->tokens);

    parser->blockDepth++;
    int stackDepthStart = parser->stackDepth;
    while (peekAtToken(parser->tokens).type != TOKEN_RIGHT_BRACE)
    {
        statement(parser);
    }

    int numLocals = parser->stackDepth - stackDepthStart;
    for (int i = 0; i < numLocals; i++)
    {
        writeChunk(parser->compiling, OP_POP);
    }

    parser->stackDepth = stackDepthStart;
    parser->blockDepth--;
    popToken(parser->tokens);
}

static void ifStatement(Parser *parser)
{
    popToken(parser->tokens);
    popToken(parser->tokens);

    expression(parser);

    writeChunk(parser->compiling, OP_JUMP_IF_FALSE);
    int currentLocation = parser->compiling->count;
    writeShort(parser->compiling, UINT16_MAX);

    popToken(parser->tokens);
    blockStatement(parser);

    overwriteShort(parser->compiling, currentLocation, parser->compiling->count);
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
        variableAssignment(parser);
    }
    else if (peeked.type == TOKEN_LEFT_BRACE)
    {
        blockStatement(parser);
    }
    else if (peeked.type == TOKEN_IF)
    {
        ifStatement(parser);
    }
    else
    {
        expression(parser);
    }
}

void compile(Chunk *chunk, const char *sourceCode)
{
    TokenArray tokens = parseTokens(sourceCode);
    TokenArrayIterator iterator = tokensIterator(tokens);

    Parser parser;
    parser.compiling = chunk;
    parser.tokens = &iterator;
    parser.stackDepth = 0;
    parser.blockDepth = -1;

    while (hasNextToken(parser.tokens))
    {
        statement(&parser);
    }

    writeChunk(chunk, OP_RETURN);
}

static void expression(Parser *parser)
{
    parseExpression(PREC_ASSIGNMENT, parser);
}

static void parseExpression(Precedence precedence, Parser *parser)
{
    Token prefixToken = peekAtToken(parser->tokens);
    ParseFn prefixFunction = getRule(prefixToken.type)->prefix;

    prefixFunction(parser);

    Token maybeInfixToken = peekAtToken(parser->tokens);
    ParseRule *infixParseRule = getRule(maybeInfixToken.type);
    while (hasNextToken(parser->tokens) && !isAtEndOfStatement(parser) && precedence <= infixParseRule->Precedence)
    {
        ParseFn infixFunction = getRule(maybeInfixToken.type)->infix;
        infixFunction(parser);
        maybeInfixToken = peekAtToken(parser->tokens);
    }
}

static bool isAtEndOfStatement(Parser *parser)
{
    return peekAtToken(parser->tokens).type == TOKEN_SEMICOLON;
}