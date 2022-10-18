// Now what is interesting is that you would think this should be called a Lexer and not a Scanner.
package com.example;
import static org.junit.Assert.assertEquals;

import java.util.List;

import org.junit.Test;

public class ScannerTest {
    @Test
    public void testTokenizeString() {
        String source = "\"Cody\"";
        Scanner scanner = new Scanner(source);

        List<Token> tokens = scanner.scanTokens();
        assertEquals(1, tokens.size());

        Token token = tokens.get(0);
        assertEquals(TokenType.STRING, token.type);
    }

    @Test
    public void testTokenizeVariableAssignment() {
        String source = "var identifier = \"Cody\";";
        Scanner scanner = new Scanner(source);

        List<Token> tokens = scanner.scanTokens();
        assertEquals(5, tokens.size());

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
        assertEquals(4, tokens.size());

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
}
