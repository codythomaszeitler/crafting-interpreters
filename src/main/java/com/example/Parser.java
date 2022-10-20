package com.example;

import java.util.ArrayList;
import java.util.List;

public class Parser {

    private int current;

    private final List<Token> tokens;

    public Parser(List<Token> tokens) {
        this.tokens = tokens;
    }

    public List<Stmt> parse() {
        List<Stmt> statements = new ArrayList<>();

        while (!isAtEnd()) {
            statements.add(statement());
        }

        return statements;
    }

    private Boolean isAtEnd() {
        return this.tokens.get(current).type == TokenType.EOF;
    }

    private Stmt statement() {
        Token maybePrint = peek();
        if (maybePrint.type == TokenType.PRINT) {
            advance();
            Expr expression = expression();
            Stmt statement = new Stmt.Print(expression);
            advance();
            return statement;
        } else {
            Expr expression = expression();
            advance();
            return new Stmt.Expression(expression);
        }
    }

    private Expr expression() {
        return equality();
    }

    private Expr equality() {
        return comparison();
    }

    private Expr comparison() {
        return term();
    }

    private Expr term() {
        Expr left = factor();
        if (match(TokenType.PLUS, TokenType.MINUS)) {
            Token operator = advance();
            Expr right = factor();

            return new Expr.Binary(left, operator, right);
        }
        return left;
    }

    private Expr factor() {
        return unary();
    }

    private Expr unary() {
        return primary();
    }

    private Expr primary() {
        Token number = advance();
        return new Expr.Literal(Double.parseDouble(number.lexeme));
    }

    private Token peek() {
        return this.tokens.get(current);
    }

    private Token advance() {
        return this.tokens.get(current++);
    }

    private Boolean match(TokenType... types) {
        Token next = peek();
        for (TokenType type : types) {
            if (type == next.type) {
                return true;
            }
        }
        return false;
    }
}
