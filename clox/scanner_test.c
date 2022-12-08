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
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(testItShouldParsePrintExpression);
    RUN_TEST(testItShouldParseAddition);
    return UNITY_END();
}
