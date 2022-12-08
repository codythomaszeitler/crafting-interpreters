#include "scanner.h"
#include <stdlib.h>
#include <stdint.h>
#include "memory.h"
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct Lexer
{
    uint32_t current;
    uint32_t iterator;
    const char *sourceCode;
} Lexer;

static void initLexer(Lexer *lexer, const char *sourceCode)
{
    lexer->current = 0;
    lexer->iterator = 0;
    lexer->sourceCode = sourceCode;
}

static void writeToken(TokenArray *tokenArray, Token token)
{
    if (tokenArray->capacity < tokenArray->count + 1)
    {
        int oldCapacity = tokenArray->capacity;
        tokenArray->capacity = GROW_CAPACITY(tokenArray->capacity);
        tokenArray->tokens = GROW_ARRAY(Token, tokenArray->tokens, oldCapacity, tokenArray->capacity);
    }

    tokenArray->tokens[tokenArray->count] = token;
    tokenArray->count = tokenArray->count + 1;
}

static void initTokenArray(TokenArray *);
static bool hasNext(Lexer *lexer);
static char peek(Lexer *lexer);
static char pop(Lexer *lexer);
static void identifierOrKeyword(Lexer *, TokenArray *);
static void semicolon(Lexer *, TokenArray *);
static void chewUpWhitespace(Lexer *);
static void add(Lexer *, TokenArray *);
static void digit(Lexer *, TokenArray *);

TokenArray parseTokens(const char *sourceCode)
{
    TokenArray tokenArray;
    initTokenArray(&tokenArray);

    Lexer lexer;
    initLexer(&lexer, sourceCode);

    while (hasNext(&lexer))
    {
        chewUpWhitespace(&lexer);

        char current = peek(&lexer);
        if (isalpha(current))
        {
            identifierOrKeyword(&lexer, &tokenArray);
        }
        else if (current == ';')
        {
            semicolon(&lexer, &tokenArray);
        }
        else if (isdigit(current))
        {
            digit(&lexer, &tokenArray);
        }
        else if (current == '+')
        {
            add(&lexer, &tokenArray);
        }
        else
        {
            printf("%c was not supported by parse tokens\n", current);
            pop(&lexer);
        }
    }

    return tokenArray;
}

static void initTokenArray(TokenArray *tokenArray)
{
    tokenArray->capacity = 0;
    tokenArray->count = 0;
    tokenArray->tokens = NULL;
}

static bool hasNext(Lexer *lexer)
{
    return peek(lexer) != '\0';
}

static char peek(Lexer *lexer)
{
    return lexer->sourceCode[lexer->current];
}

static char pop(Lexer *lexer)
{
    char popped = lexer->sourceCode[lexer->current];
    lexer->current = lexer->current + 1;
    return popped;
}

static char *substring(Lexer *lexer, int start, int end)
{
    char *lexeme = malloc(end - start + 1);

    for (int i = start; i < end; i++)
    {
        lexeme[i - start] = lexer->sourceCode[i];
    }
    lexeme[end - start] = '\0';
    return lexeme;
}

static void identifierOrKeyword(Lexer *lexer, TokenArray *tokenArray)
{
    int start = lexer->current;
    while (isalnum(peek(lexer)))
    {
        pop(lexer);
    }
    int end = lexer->current;

    char *lexeme = substring(lexer, start, end);
    int lexemeLength = strlen(lexeme);

    TokenType type = -1;
    if (lexemeLength == 5)
    {
        if (!strcmp("print", lexeme))
        {
            type = TOKEN_PRINT;
        }
    }

    if (type == -1)
    {
        type = TOKEN_IDENTIFIER;
    }

    Token token;
    token.lexeme = lexeme;
    token.location.start = start;
    token.location.end = end;
    token.location.line = -1;
    token.type = type;

    writeToken(tokenArray, token);
}

static void chewUpWhitespace(Lexer *lexer)
{
    while (isspace(peek(lexer)))
    {
        pop(lexer);
    }
}

static Token consumerSingleCharacter(Lexer *lexer, TokenType type) {
    int start = lexer->current;
    pop(lexer);
    int end = lexer->current;

    char *lexeme = substring(lexer, start, end);

    Token token;
    token.lexeme = lexeme;
    token.location.start = start;
    token.location.end = end;
    token.type = type;
    return token;
}

static void semicolon(Lexer *lexer, TokenArray *tokenArray)
{
    writeToken(tokenArray, consumerSingleCharacter(lexer, TOKEN_SEMICOLON));
}

static void add(Lexer *lexer, TokenArray *tokenArray)
{
    writeToken(tokenArray, consumerSingleCharacter(lexer, TOKEN_PLUS));
}

static void digit(Lexer *lexer, TokenArray *tokenArray)
{
    writeToken(tokenArray, consumerSingleCharacter(lexer, TOKEN_NUMBER));
}