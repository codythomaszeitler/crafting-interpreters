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

void scanner_initLexer(Lexer *lexer, const char *sourceCode)
{
    lexer->current = 0;
    lexer->iterator = 0;
    lexer->sourceCode = sourceCode;
}

Token *scanner_heapToken(Token token)
{
    Token *onHeap = malloc(sizeof(token));
    onHeap->lexeme = token.lexeme;
    onHeap->location.start = token.location.start;
    onHeap->location.end = token.location.end;
    onHeap->type = token.type;
    return onHeap;
}

void scanner_writeToken(TokenArray *tokenArray, Token token)
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

void scanner_initTokenArray(TokenArray *);
bool scanner_hasNext(Lexer *lexer);
char scanner_peek(Lexer *lexer);
char scanner_pop(Lexer *lexer);
void scanner_identifierOrKeyword(Lexer *, TokenArray *);
void scanner_semicolon(Lexer *, TokenArray *);
void scanner_chewUpWhitespace(Lexer *);
void scanner_add(Lexer *, TokenArray *);
void scanner_digit(Lexer *, TokenArray *);

TokenArray parseTokens(const char *sourceCode)
{
    TokenArray tokenArray;
    scanner_initTokenArray(&tokenArray);

    Lexer lexer;
    scanner_initLexer(&lexer, sourceCode);

    while (scanner_hasNext(&lexer))
    {
        scanner_chewUpWhitespace(&lexer);

        char current = scanner_peek(&lexer);
        if (isalpha(current))
        {
            scanner_identifierOrKeyword(&lexer, &tokenArray);
        }
        else if (current == ';')
        {
            scanner_semicolon(&lexer, &tokenArray);
        }
        else if (isdigit(current))
        {
            scanner_digit(&lexer, &tokenArray);
        }
        else if (current == '+')
        {
            scanner_add(&lexer, &tokenArray);
        }
        else
        {
            printf("%c was not supported by parse tokens\n", current);
            scanner_pop(&lexer);
        }
    }

    return tokenArray;
}

void scanner_initTokenArray(TokenArray *tokenArray)
{
    tokenArray->capacity = 0;
    tokenArray->count = 0;
    tokenArray->tokens = NULL;
}

bool scanner_hasNext(Lexer *lexer)
{
    return scanner_peek(lexer) != '\0';
}

char scanner_peek(Lexer *lexer)
{
    return lexer->sourceCode[lexer->current];
}

char scanner_pop(Lexer *lexer)
{
    char popped = lexer->sourceCode[lexer->current];
    lexer->current = lexer->current + 1;
    return popped;
}

char *scanner_substring(Lexer *lexer, int start, int end)
{
    char *lexeme = malloc(end - start + 1);

    for (int i = start; i < end; i++)
    {
        lexeme[i - start] = lexer->sourceCode[i];
    }
    lexeme[end - start] = '\0';
    return lexeme;
}

void scanner_identifierOrKeyword(Lexer *lexer, TokenArray *tokenArray)
{
    int start = lexer->current;
    while (isalnum(scanner_peek(lexer)))
    {
        scanner_pop(lexer);
    }
    int end = lexer->current;

    char *lexeme = scanner_substring(lexer, start, end);
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

    scanner_writeToken(tokenArray, token);
}

bool scanner_isAlphaNum(char toCheck)
{
    return isalnum(toCheck);
}

void scanner_chewUpWhitespace(Lexer *lexer)
{
    while (isspace(scanner_peek(lexer)))
    {
        scanner_pop(lexer);
    }
}

Token scanner_consumerSingleCharacter(Lexer *lexer, TokenType type) {
    int start = lexer->current;
    scanner_pop(lexer);
    int end = lexer->current;

    char *lexeme = scanner_substring(lexer, start, end);

    Token token;
    token.lexeme = lexeme;
    token.location.start = start;
    token.location.end = end;
    token.type = type;
    return token;
}

void scanner_semicolon(Lexer *lexer, TokenArray *tokenArray)
{
    scanner_writeToken(tokenArray, scanner_consumerSingleCharacter(lexer, TOKEN_SEMICOLON));
}

void scanner_add(Lexer *lexer, TokenArray *tokenArray)
{
    scanner_writeToken(tokenArray, scanner_consumerSingleCharacter(lexer, TOKEN_PLUS));
}

void scanner_digit(Lexer *lexer, TokenArray *tokenArray)
{
    scanner_writeToken(tokenArray, scanner_consumerSingleCharacter(lexer, TOKEN_NUMBER));
}