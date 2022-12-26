#include "unity.h"
#include "scanner.h"

void setUp() {}

void tearDown() {}

void testItShouldParsePrintExpression()
{
    const char *sourceCode = "print cody123;";
    TokenArray tokenArray = parseTokens(sourceCode);
    TEST_ASSERT_EQUAL(3, tokenArray.count);

    TEST_ASSERT_EQUAL_STRING("print", tokenArray.tokens[0].lexeme);
    TEST_ASSERT_EQUAL(TOKEN_PRINT, tokenArray.tokens[0].type);

    TEST_ASSERT_EQUAL_STRING("cody123", tokenArray.tokens[1].lexeme);
    TEST_ASSERT_EQUAL(TOKEN_IDENTIFIER, tokenArray.tokens[1].type);

    TEST_ASSERT_EQUAL_STRING(";", tokenArray.tokens[2].lexeme);
    TEST_ASSERT_EQUAL(TOKEN_SEMICOLON, tokenArray.tokens[2].type);
}

void testItShouldParseAddition()
{
    const char *sourceCode = "print 1 + 2;";
    TokenArray tokenArray = parseTokens(sourceCode);
    TEST_ASSERT_EQUAL(5, tokenArray.count);

    TEST_ASSERT_EQUAL_STRING("print", tokenArray.tokens[0].lexeme);
    TEST_ASSERT_EQUAL(TOKEN_PRINT, tokenArray.tokens[0].type);

    TEST_ASSERT_EQUAL_STRING("1", tokenArray.tokens[1].lexeme);
    TEST_ASSERT_EQUAL(TOKEN_NUMBER, tokenArray.tokens[1].type);

    TEST_ASSERT_EQUAL_STRING("+", tokenArray.tokens[2].lexeme);
    TEST_ASSERT_EQUAL(TOKEN_PLUS, tokenArray.tokens[2].type);

    TEST_ASSERT_EQUAL_STRING("2", tokenArray.tokens[3].lexeme);
    TEST_ASSERT_EQUAL(TOKEN_NUMBER, tokenArray.tokens[3].type);

    TEST_ASSERT_EQUAL_STRING(";", tokenArray.tokens[4].lexeme);
    TEST_ASSERT_EQUAL(TOKEN_SEMICOLON, tokenArray.tokens[4].type);

    TokenArrayIterator iterator = tokensIterator(tokenArray);
    Token printToken = popToken(&iterator);
    TEST_ASSERT_EQUAL_STRING("print", printToken.lexeme);
    TEST_ASSERT_EQUAL(1, iterator.current);

    TEST_ASSERT_TRUE(hasNextToken(&iterator));
    Token numberOneToken = peekAtToken(&iterator);
    TEST_ASSERT_EQUAL_STRING("1", numberOneToken.lexeme);
    TEST_ASSERT_TRUE(hasNextToken(&iterator));

    Token poppedNumberOneToken = popToken(&iterator);
    TEST_ASSERT_EQUAL_STRING("1", poppedNumberOneToken.lexeme);
    TEST_ASSERT_TRUE(hasNextToken(&iterator));

    Token addToken = popToken(&iterator);
    TEST_ASSERT_EQUAL_STRING("+", addToken.lexeme);
    TEST_ASSERT_TRUE(hasNextToken(&iterator));

    Token numberTwoToken = popToken(&iterator);
    TEST_ASSERT_EQUAL_STRING("2", numberTwoToken.lexeme);
    TEST_ASSERT_TRUE(hasNextToken(&iterator));

    Token semicolon = popToken(&iterator);
    TEST_ASSERT_EQUAL_STRING(";", semicolon.lexeme);
    TEST_ASSERT_FALSE(hasNextToken(&iterator));
}

void testItShouldBeAbleToParseLessThanOrEquals()
{
    const char *sourceCode = "print 1 <= 2;";
    TokenArray tokenArray = parseTokens(sourceCode);
    TEST_ASSERT_EQUAL(5, tokenArray.count);

    TokenArrayIterator iterator = tokensIterator(tokenArray);
    Token printToken = popToken(&iterator);
    TEST_ASSERT_EQUAL_STRING("print", printToken.lexeme);

    Token numberOneToken = popToken(&iterator);
    TEST_ASSERT_EQUAL_STRING("1", numberOneToken.lexeme);

    Token lessThanOrEqualsToken = popToken(&iterator);
    TEST_ASSERT_EQUAL_STRING("<=", lessThanOrEqualsToken.lexeme);
    TEST_ASSERT_EQUAL(TOKEN_LESS_EQUAL, lessThanOrEqualsToken.type);

    Token numberTwoToken = popToken(&iterator);
    TEST_ASSERT_EQUAL_STRING("2", numberTwoToken.lexeme);

    Token semicolon = popToken(&iterator);
    TEST_ASSERT_EQUAL_STRING(";", semicolon.lexeme);
}

void testItShouldBeAbleToParseLessThan()
{
    const char *sourceCode = "print 1 < 2;";
    TokenArray tokenArray = parseTokens(sourceCode);
    TEST_ASSERT_EQUAL(5, tokenArray.count);

    TokenArrayIterator iterator = tokensIterator(tokenArray);
    Token printToken = popToken(&iterator);
    TEST_ASSERT_EQUAL_STRING("print", printToken.lexeme);

    Token numberOneToken = popToken(&iterator);
    TEST_ASSERT_EQUAL_STRING("1", numberOneToken.lexeme);

    Token lessThanToken = popToken(&iterator);
    TEST_ASSERT_EQUAL_STRING("<", lessThanToken.lexeme);
    TEST_ASSERT_EQUAL(TOKEN_LESS, lessThanToken.type);

    Token numberTwoToken = popToken(&iterator);
    TEST_ASSERT_EQUAL_STRING("2", numberTwoToken.lexeme);

    Token semicolon = popToken(&iterator);
    TEST_ASSERT_EQUAL_STRING(";", semicolon.lexeme);
}

void testItShouldBeAbleToDoOr() 
{
    const char *sourceCode = "print true || false;";
    TokenArray tokenArray = parseTokens(sourceCode);
    TEST_ASSERT_EQUAL(5, tokenArray.count);

    TokenArrayIterator iterator = tokensIterator(tokenArray);
    Token printToken = popToken(&iterator);
    TEST_ASSERT_EQUAL_STRING("print", printToken.lexeme);

    Token numberOneToken = popToken(&iterator);
    TEST_ASSERT_EQUAL_STRING("true", numberOneToken.lexeme);

    Token lessThanToken = popToken(&iterator);
    TEST_ASSERT_EQUAL_STRING("||", lessThanToken.lexeme);
    TEST_ASSERT_EQUAL(TOKEN_OR, lessThanToken.type);

    Token numberTwoToken = popToken(&iterator);
    TEST_ASSERT_EQUAL_STRING("false", numberTwoToken.lexeme);

    Token semicolon = popToken(&iterator);
    TEST_ASSERT_EQUAL_STRING(";", semicolon.lexeme);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(testItShouldParsePrintExpression);
    RUN_TEST(testItShouldParseAddition);
    RUN_TEST(testItShouldBeAbleToParseLessThanOrEquals);
    RUN_TEST(testItShouldBeAbleToParseLessThan);
    RUN_TEST(testItShouldBeAbleToDoOr);
    return UNITY_END();
}
