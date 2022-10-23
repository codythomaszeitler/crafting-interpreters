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
        Token maybeVarOrBlockStmtOrIfStmt = peek();
        if (maybeVarOrBlockStmtOrIfStmt.type == TokenType.IF) {
            return ifStatement();
        } else if (maybeVarOrBlockStmtOrIfStmt.type == TokenType.LEFT_BRACE) {
            return blockStatement();
        } else if (maybeVarOrBlockStmtOrIfStmt.type == TokenType.VAR) {
            return varDeclaration();
        } else {
            return statement();
        }
    }

    private Stmt ifStatement() {
        Token ifToken = advance();
        Token leftParen = advance();
        Expr expression = expression();
        Token rightParent = advance();
        Stmt.Block blockStatement = blockStatement();

        Stmt.If ifStatement = new Stmt.If(expression, blockStatement);
        return ifStatement;
    }

    private Stmt.Block blockStatement() {
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

    // You need to call yourself or something with higher precedence? 
    // The reason that works is because stuff at the lowest part of the precedence
    // tree becomes the root? or something of that sort. 
    private Expr expression() {
        return assignment();
    }

    private Expr assignment() {
        Expr maybeAssignmentToName = equality();

        Token maybeEquals = peek();
        if (maybeEquals.type == TokenType.EQUAL) {
            Token equals = advance();
            Expr rightHandSide = assignment();

            if (maybeAssignmentToName instanceof Expr.Variable) {
                Expr.Variable leftHandSideName = (Expr.Variable) maybeAssignmentToName;
                Token name = leftHandSideName.name;

                return new Expr.Assign(name, rightHandSide);
            }
            return null;
        }
        return maybeAssignmentToName;
    }

    // This might be an algorithm that we constantly have to keep coming back to.
    private Expr equality() {
        // Okay here is the equality thing...

        // How does it handle multiple left associativity and WHY 
        // does it work before I blindly just TDD a solution.

        // The parser is probably ready to consume a 5.
        // The parser is actually more ready to consumer a comparison... 
        // which may end up being a primary.
        // (Remember the mother flipping tree my fam) 
        Expr leftHandSide = comparison();

        // So at this point we never should have parsed a == 
        // So we should probably be able to do that right now...
        // I wonder if the scanner can actually handle this at this point.

        Token maybeDoubleEquals = peek();
        if (TokenType.EQUAL_EQUAL == maybeDoubleEquals.type) {
            Token doubleEquals = advance();
            Expr rightHandSide = comparison();

            return new Expr.Binary(leftHandSide, doubleEquals, rightHandSide);
        } else {
            return leftHandSide;
        }
    }

    private Expr comparison() {
        return term();
    }

    private Expr term() {
        Expr left = factor();
        if (match(TokenType.PLUS, TokenType.MINUS)) {
            Token operator = advance();
            Expr right = factor();

            // We do already have the operator here... 
            return new Expr.Binary(left, operator, right);
        }
        return left;
    }

    private Expr factor() {
        return unary();
    }

    private Expr unary() {
        Token maybeUnaryOperator = peek();
        if (maybeUnaryOperator.type == TokenType.BANG) {
            Token unaryOperator = advance();
            Expr toBang = expression();
            return new Expr.Unary(unaryOperator, toBang);
        } else {
            return primary();
        }
    }

    private Expr primary() {
        Token first = advance();

        if (first.type == TokenType.STRING) {
            return new Expr.Literal(first.lexeme);
        } else if (first.type == TokenType.TRUE || first.type == TokenType.FALSE) {
            return new Expr.Literal(Boolean.parseBoolean(first.lexeme));
        } else if (first.type == TokenType.NUMBER) {
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
