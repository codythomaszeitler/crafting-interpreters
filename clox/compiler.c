#include "compiler.h"
#include "chunk.h"
#include "scanner.h"
#include "vm.h"
#include <stdlib.h>

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
static void expression(Parser *);
static void parseExpression(Precedence, Parser *);

ParseRule rules[] = {
    [TOKEN_NUMBER] = {number, NULL, PREC_NONE},
    [TOKEN_PLUS] = {NULL, binary, PREC_TERM},
    [TOKEN_MINUS] = {unary, binary, PREC_UNARY},
    [TOKEN_STAR] = {NULL, binary, PREC_FACTOR},
    [TOKEN_SLASH] = {NULL, binary, PREC_FACTOR}};

static ParseRule *getRule(TokenType tokenType)
{
    return &rules[tokenType];
}

static void number(Parser *parser)
{
    Token shouldBeNumber = popToken(parser->tokens);

    Value value = strtod(shouldBeNumber.lexeme, NULL);

    int valueConstantIndex = addConstant(parser->compiling, value);
    writeChunk(parser->compiling, OP_CONSTANT);
    writeChunk(parser->compiling, valueConstantIndex);
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
    interpret(&interpreter->vm, &bytecode);
}

void compile(Chunk *chunk, const char *sourceCode)
{
    TokenArray tokens = parseTokens(sourceCode);
    TokenArrayIterator iterator = tokensIterator(tokens);

    Parser parser;
    parser.compiling = chunk;
    parser.tokens = &iterator;

    expression(&parser);
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
    while (hasNextToken(parser->tokens) && precedence <= infixParseRule->Precedence)
    {
        ParseFn infixFunction = getRule(maybeInfixToken.type)->infix;
        infixFunction(parser);
        maybeInfixToken = peekAtToken(parser->tokens);
    }
}
