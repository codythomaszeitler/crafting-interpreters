#include "compiler.h"
#include "chunk.h"
#include "scanner.h"
#include "vm.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct Parser
{
    TokenArrayIterator *tokens;
    Chunk *compiling;
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
static void expression(Parser *);
static void parseExpression(Precedence, Parser *);
static bool isAtEndOfStatement(Parser *parser);

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
        writeChunk(parser->compiling, OP_VAR_EXPRESSION);
        writeString(parser->compiling, (const char *)shouldBeTrue.lexeme);
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

static void variableDecl(Parser *parser)
{
    popToken(parser->tokens);
    Token identifier = popToken(parser->tokens);
    // So this bytecode... would look like
    // OP_VAR_DECL a;
    writeChunk(parser->compiling, OP_VAR_DECL);
    writeString(parser->compiling, (const char *)identifier.lexeme);
    popToken(parser->tokens);
}

static void variableAssignment(Parser *parser)
{
    Token identifier = popToken(parser->tokens);
    popToken(parser->tokens);

    expression(parser);

    writeChunk(parser->compiling, OP_VAR_ASSIGN);
    writeString(parser->compiling, (const char *)identifier.lexeme);
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
        variableAssignment(parser);
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

    while (hasNextToken(parser.tokens))
    {
        statement(&parser);
    }
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