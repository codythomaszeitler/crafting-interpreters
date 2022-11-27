package com.example;

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
        reserved.put("true", TokenType.TRUE);
        reserved.put("false", TokenType.FALSE);
        reserved.put("if", TokenType.IF);
        reserved.put("func", TokenType.FUN);
        reserved.put("return", TokenType.RETURN);
    }

    private int current;
    private int start;
    private int line;
    private String source;
    private Boolean hasError;

    private List<Token> tokens;
    private Reporter reporter;

    public Scanner(String source) {
        this(source, new SysOutReporter());
    }

    public Scanner(String source, Reporter reporter) {
        this.start = 0;
        this.current = 0;
        this.line = 1;
        this.source = source;
        this.tokens = new ArrayList<>();
        this.hasError = false;
        this.reporter = reporter;
    }

    public List<Token> scanTokens() {
        while (!isAtEnd()) {

            chewUpWhitespace();

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
            } else if (startingChar == '(') {
                leftParen();
            } else if (startingChar == ')') {
                rightParen();
            } else if (startingChar == '>') {
                greaterThan();
            } else if (startingChar == '<') {
                lessThanOrLessThanOrEqualTo();
            } else if (startingChar == '-') {
                subtraction();
            } else if (startingChar == ',') {
                comma();
            } else if (startingChar == '&') {
                and();
            } else if (startingChar == '!') {
                bang();
            } else if (isAlpha(startingChar)) {
                identifierOrKeyword();
            } else if (isQuote(startingChar)) {
                string();
            } else if (isDigit(startingChar)) {
                digit();
            } else {
                startingChar = advance();
                this.hasError = true;

                this.reporter.reportError(new ScannerErrorParams(startingChar, this.line));
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

    private void leftParen() {
        char leftParen = advance();
        Token token = new Token(TokenType.LEFT_PAREN, "(", leftParen, this.line);
        this.tokens.add(token);
    }

    private void rightParen() {
        char rightParen = advance();
        Token token = new Token(TokenType.RIGHT_PAREN, ")", rightParen, this.line);
        this.tokens.add(token);
    }

    private void greaterThan() {
        char greaterThan = advance();
        Token token = new Token(TokenType.GREATER, ">", greaterThan, this.line);
        this.tokens.add(token);
    }

    private void lessThanOrLessThanOrEqualTo() {
        advance();
        char maybeEquals = peek();

        if (maybeEquals == '=') {
            advance();
            Token token = new Token(TokenType.LESS_EQUAL, "<=", "<=", this.line);
            this.tokens.add(token);
        } else {
            Token token = new Token(TokenType.GREATER, "<", "<", this.line);
            this.tokens.add(token);
        }
    }

    private void subtraction() {
        char subtraction = advance();
        Token token = new Token(TokenType.MINUS, "-", subtraction, this.line);
        this.tokens.add(token);
    }

    private void comma() {
        char comma = advance();
        Token token = new Token(TokenType.COMMA, ",", comma, this.line);
        this.tokens.add(token);
    }

    private void and() {
        String and = advance() + advance() + "";
        Token token = new Token(TokenType.AND, "&&", and, this.line);
        this.tokens.add(token);
    }

    private void bang() {
        char bang = advance();
        Token token = new Token(TokenType.BANG, "!", bang, this.line);
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

    public static class ScannerErrorParams {
        private final char unexpected;
        private final int line;

        private ScannerErrorParams(char unexpected, int line) {
            this.unexpected = unexpected;
            this.line = line;
        }

        public String getErrorMessage() {
            String formatString = "Lox compile error: Unexpected character \"%c\" found at line %d";
            return String.format(formatString, new Object[]{this.unexpected, this.line});
        }
    }

    public static interface Reporter {
        public void reportError(ScannerErrorParams params);
    }

    public static class SysOutReporter implements Reporter {
        @Override
        public void reportError(ScannerErrorParams params) {
            System.out.println(params.getErrorMessage());
        }
    }
}
