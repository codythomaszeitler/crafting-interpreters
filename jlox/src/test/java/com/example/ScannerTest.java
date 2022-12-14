// Now what is interesting is that you would think this should be called a Lexer and not a Scanner.
package com.example;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

import java.util.List;

import org.junit.Test;
import org.junit.internal.runners.TestMethod;

public class ScannerTest {
    @Test
    public void testTokenizeString() {
        String source = "\"Cody\"";
        Scanner scanner = new Scanner(source);

        List<Token> tokens = scanner.scanTokens();
        assertEquals(2, tokens.size());

        Token token = tokens.get(0);
        assertEquals(TokenType.STRING, token.type);

        Token eof = tokens.get(1);
        assertEquals(TokenType.EOF, eof.type);
    }

    @Test
    public void testTokenizeVariableAssignment() {
        String source = "var identifier = \"Cody\";";
        Scanner scanner = new Scanner(source);

        List<Token> tokens = scanner.scanTokens();
        assertEquals(6, tokens.size());

        // The tokens should be necessarily ordered.
        Token varToken = tokens.get(0);
        assertEquals(TokenType.VAR, varToken.type);
    }

    @Test
    public void testTokenizeNotEquals() {
        // So you need the semicolon to make a valid expression?
        // This does not actually make that much sense... but let's keep going.
        String source = "1 == 2;";
        Scanner scanner = new Scanner(source);

        List<Token> tokens = scanner.scanTokens();
        assertEquals(5, tokens.size());

        Token oneToken = tokens.get(0);
        assertEquals(TokenType.NUMBER, oneToken.type);
        assertEquals("1", oneToken.lexeme);

        Token twoToken = tokens.get(2);
        assertEquals(TokenType.NUMBER, twoToken.type);
        assertEquals("2", twoToken.lexeme);

        Token equalsToken = tokens.get(1);
        assertEquals(TokenType.EQUAL_EQUAL, equalsToken.type);
        assertEquals("==", equalsToken.lexeme);
    }

    @Test
    public void testTokenizeTrueBoolean() {
        String source = "var cody = true;";
        Scanner testObject = new Scanner(source);

        List<Token> tokens = testObject.scanTokens();
        Token booleanToken = tokens.get(3);
        assertEquals(TokenType.TRUE, booleanToken.type);
    }

    @Test
    public void testTokenizeFalseBoolean() {
        String source = "var cody = false;";
        Scanner testObject = new Scanner(source);

        List<Token> tokens = testObject.scanTokens();
        Token booleanToken = tokens.get(3);
        assertEquals(TokenType.FALSE, booleanToken.type);
    }

    @Test
    public void testTokenizeIfStatement() {
        String source = "var cody = false; if (cody) {print \"test\"; }";
        Scanner testObject = new Scanner(source);

        List<Token> tokens = testObject.scanTokens();
        Token ifToken = tokens.get(5);
        assertEquals(TokenType.IF, ifToken.type);

        Token leftParen = tokens.get(6);
        assertEquals(TokenType.LEFT_PAREN, leftParen.type);

        Token rightParen = tokens.get(8);
        assertEquals(TokenType.RIGHT_PAREN, rightParen.type);

        Token expressionToken = tokens.get(7);
        assertEquals(TokenType.IDENTIFIER, expressionToken.type);
    }

    @Test
    public void itShouldBeAbleToParseAUnaryOperator() {
        String source = "var cody = !false;";
        Scanner testObject = new Scanner(source);

        List<Token> tokens = testObject.scanTokens();
        assertEquals(7, tokens.size());

        Token unaryOperator = tokens.get(3);
        assertEquals(TokenType.BANG, unaryOperator.type);
    }

    @Test
    public void itShouldParseAndBooleanOperator() {
        String source = "true && false";
        Scanner testObject = new Scanner(source);

        List<Token> tokens = testObject.scanTokens();
        assertEquals(TokenType.AND, tokens.get(1).type);
    }

    @Test
    public void itShouldLogErrorWithBadToken() {
        TestScannerReporter reporter = new TestScannerReporter();

        String source = "as ` 45";
        Scanner testObject = new Scanner(source, reporter);
        testObject.scanTokens();

        assertTrue(reporter.hasErrorMessage("Lox compile error: Unexpected character \"`\" found at line 1"));
    }

    @Test
    public void itShouldScanWithComma() {
        String source = "print \"Hello, World!\";";
        Scanner testObject = new Scanner(source);

        List<Token> tokens = testObject.scanTokens();
        assertEquals(4, tokens.size());

        Token shouldBeHelloWorld = tokens.get(1);
        assertEquals(TokenType.STRING, shouldBeHelloWorld.type);

        assertEquals("Hello, World!", shouldBeHelloWorld.lexeme);
    }
}
