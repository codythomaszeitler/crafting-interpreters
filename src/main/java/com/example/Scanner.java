package com.example;

// How do we handle whitespace?
// removing all the whitespace after you've done tokenizing something 
// is probably really important and not something you always have to do
// but probably is a general problem that you need to solve.

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class Scanner {

    private static Map<String, TokenType> reserved;

    static {
        reserved = new HashMap<>();
        reserved.put("var", TokenType.VAR);
        reserved.put("print", TokenType.PRINT);
    }

    private int current;
    private int start;
    private int line;
    private String source;
    private Boolean hasError;

    private List<Token> tokens;
    private ErrorReport reporter;

    public Scanner(String source) {
        this.start = 0;
        this.current = 0;
        this.source = source;
        this.tokens = new ArrayList<>();
        this.hasError = false;
        this.reporter = new SysOutErrorReport();
    }

    public List<Token> scanTokens() {
        while (!isAtEnd()) {

            chewUpWhitespace();

            // So we have already technically read the character at this point
            this.start = this.current;
            char startingChar = this.peek();

            if (startingChar == '=') {
                equalsOrDoubleEquals();
            } else if (startingChar == ';') {
                semicolon();
            } else if (startingChar == '+') {
                add();
            } else if (startingChar == '{') {
                blockStart();
            } else if (startingChar == '}') {
                blockEnd();
            } else if (isAlpha(startingChar)) {
                // Now this is where it gets interesting. We are going to use a STATEMENT (not
                // an expression ;)
                // This can either be an identifier or a keyword?
                identifierOrKeyword();
            } else if (isQuote(startingChar)) {
                string();
            } else if (isDigit(startingChar)) {
                digit();
            } else {
                startingChar = advance();
                // This is the error case?
                this.hasError = true;
                this.reporter.report(this.line, "", "Unexpected character found: [" + startingChar + "]");
            }
        }

        Token eof = new Token(TokenType.EOF, "", null, this.line);
        this.tokens.add(eof);

        return this.tokens;
    }

    private void chewUpWhitespace() {
        while (isWhitespace(peek())) {
            advance();
        }
    }

    private void equalsOrDoubleEquals() {
        // So this will always be an equals character.
        advance();
        char maybeSecondEquals = peek();
        if (maybeSecondEquals == '=') {
            advance();
            Token token = new Token(TokenType.EQUAL_EQUAL, "==", null, line);
            this.tokens.add(token);
        } else {
            Token token = new Token(TokenType.EQUAL, "=", null, line);
            this.tokens.add(token);
        }
    }

    private void semicolon() {
        advance();
        Token token = new Token(TokenType.SEMICOLON, ";", null, line);
        this.tokens.add(token);
    }

    private void string() {
        advance();

        String lexeme = "";
        while (isAlphaNumeric(peek())) {
            lexeme += advance();
        }

        Token token = new Token(TokenType.STRING, lexeme, null, line);
        this.tokens.add(token);

        advance();
    }

    private Boolean isWhitespace(char toCheck) {
        return toCheck == ' ' || toCheck == '\n' || toCheck == '\t';
    }

    private Boolean isAtEnd() {
        return this.current == this.source.length();
    }

    private Boolean isAlpha(char toCheck) {
        Boolean isUpperCase = toCheck >= 'A' && toCheck <= 'Z';
        Boolean isLowerCase = toCheck >= 'a' && toCheck <= 'z';
        return isUpperCase || isLowerCase;
    }

    private void identifierOrKeyword() {
        while (isAlphaNumeric(peek())) {
            advance();
        }
        // So at this point, you have a start and a current that are in the correct
        // spots.
        String lexeme = this.source.substring(start, current);
        if (reserved.containsKey(lexeme)) {
            Token token = new Token(reserved.get(lexeme), lexeme, lexeme, line);
            this.tokens.add(token);
        } else {
            Token token = new Token(TokenType.IDENTIFIER, lexeme, lexeme, line);
            this.tokens.add(token);
        }
    }

    private Boolean isAlphaNumeric(char toCheck) {
        Boolean isAlpha = isAlpha(toCheck);
        Boolean isDigit = isDigit(toCheck);
        return isAlpha || isDigit;
    }

    private Boolean isDigit(char toCheck) {
        return Character.isDigit(toCheck);
    }

    private void digit() {
        // So wait... this thing can be super long... how do we know that we are out of stuff? 
        String digitAsString = advance() + "";
        while (isDigit(peek())) {
            digitAsString += advance();
        }
        Token token = new Token(TokenType.NUMBER, digitAsString, digitAsString, this.line);
        this.tokens.add(token);
    }

    private void add() {
        char add = advance();
        Token token = new Token(TokenType.PLUS, "+", add, this.line);
        this.tokens.add(token);
    }

    private void blockStart() {
        char blockStart = advance();
        Token token = new Token(TokenType.LEFT_BRACE, "{", blockStart, this.line);
        this.tokens.add(token);
    }

    private void blockEnd() {
        char blockEnd = advance();
        Token token = new Token(TokenType.RIGHT_BRACE, "}", blockEnd, this.line);
        this.tokens.add(token);
    }

    private Boolean isQuote(char toCheck) {
        return toCheck == '"';
    }

    private char peek() {
        if (isAtEnd()) {
            return '\0';
        }
        return this.source.charAt(this.current);
    }

    private char advance() {
        if (isAtEnd()) {
            return '\0';
        }

        return this.source.charAt(this.current++);
    }
}
