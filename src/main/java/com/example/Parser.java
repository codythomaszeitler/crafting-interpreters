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
            statements.add(declaration());
        }

        return statements;
    }

    private Boolean isAtEnd() {
        return this.tokens.get(current).type == TokenType.EOF;
    }

    private Stmt declaration() {
        Token maybeVarOrBlockStart = peek();
        if (maybeVarOrBlockStart.type == TokenType.LEFT_BRACE) {
            return blockStatement();
        } else if (maybeVarOrBlockStart.type == TokenType.VAR) {
            return varDeclaration();
        } else {
            return statement();
        }
    }

    private Stmt blockStatement() {
        Token leftBrace = advance();

        List<Stmt> statements = new ArrayList<Stmt>();
        while (peek().type != TokenType.RIGHT_BRACE) {
            statements.add(declaration());
        }

        Token rightBrace = advance();
        return new Stmt.Block(statements);
    }

    private Stmt varDeclaration() {
        Token var = advance();
        Token name = advance();

        Expr initializer = null;
        if (match(TokenType.EQUAL)) {
            advance();
            initializer = expression();
        }
        advance();
        return new Stmt.Var(name, initializer);
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
        return assignment();
    }

    private Expr assignment() {

        // So this is a left hand thing of the = thingermorebob
        // WAit wait wait... hold the phone. WTF is this doing exactly?
        Expr expression = equality();

        Token maybeEquals = peek();
        if (maybeEquals.type == TokenType.EQUAL) {
            Token equals = advance();
            Expr value = assignment();

            if (expression instanceof Expr.Variable) {
                Expr.Variable variable = (Expr.Variable) expression;
                Token name = variable.name;

                return new Expr.Assign(name, value);
            }
            return null;
        }
        return expression;
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
        Token first = advance();

        if (first.type == TokenType.NUMBER) {
            return new Expr.Literal(Double.parseDouble(first.lexeme));
        } else {
            return new Expr.Variable(first);
        }
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
