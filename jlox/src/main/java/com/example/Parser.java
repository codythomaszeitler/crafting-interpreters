package com.example;

import java.util.ArrayList;
import java.util.List;

public class Parser {

    private int current;

    private final List<Token> tokens;
    private final Parser.Reporter reporter;

    public Parser(List<Token> tokens) {
        this(tokens, new Parser.SysOutReporter());
    }

    public Parser(List<Token> tokens, Parser.Reporter reporter) {
        this.tokens = tokens;
        this.reporter = reporter;
    }

    public List<Stmt> parse() {
        List<Stmt> statements = new ArrayList<>();

        while (!isAtEnd()) {
            try {
                statements.add(declaration());
            } catch (Parser.ParseError e) {
                ParserErrorParams params = new ParserErrorParams(e);
                this.reporter.reportError(params);
                synchronize();
            }
        }

        return statements;
    }

    private void synchronize() {
        while (peek().type != TokenType.EOF) {
            advance();
        }
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
        } else if (maybeVarOrBlockStmtOrIfStmt.type == TokenType.FUN) {
            return functionDeclaration();   
        } else if (maybeVarOrBlockStmtOrIfStmt.type == TokenType.RETURN) {
            return returnStatement();
        } else {
            return statement();
        }
    }

    private Stmt ifStatement() {
        consume(TokenType.IF);
        consume(TokenType.LEFT_PAREN);
        Expr expression = expression();
        consume(TokenType.RIGHT_PAREN);
        Stmt.Block blockStatement = blockStatement();

        Stmt.If ifStatement = new Stmt.If(expression, blockStatement);
        return ifStatement;
    }

    private Stmt.Block blockStatement() {
        consume(TokenType.LEFT_BRACE);

        List<Stmt> statements = new ArrayList<Stmt>();
        while (peek().type != TokenType.RIGHT_BRACE) {
            statements.add(declaration());
        }

        consume(TokenType.RIGHT_BRACE);
        return new Stmt.Block(statements);
    }

    private Stmt varDeclaration() {
        consume(TokenType.VAR);
        Token name = advance();

        Expr initializer = null;
        if (match(TokenType.EQUAL)) {
            consume(TokenType.EQUAL);
            initializer = expression();
        }
        consume(TokenType.SEMICOLON);
        return new Stmt.Var(name, initializer);
    }

    private Stmt returnStatement() {
        consume(TokenType.RETURN);
        Expr expression = expression();
        Stmt.Return stmt = new Stmt.Return(expression);
        consume(TokenType.SEMICOLON);
        return stmt;
    }

    private Stmt functionDeclaration() {
        consume(TokenType.FUN);
        Token name = advance();
        consume(TokenType.LEFT_PAREN);

        Token maybeRightParen = peek();
        if (maybeRightParen.type == TokenType.RIGHT_PAREN) {
            consume(TokenType.RIGHT_PAREN);
            Stmt functionDecl = new Stmt.FuncDecl(name, new ArrayList<String>(), blockStatement());
            consume(TokenType.SEMICOLON);
            return functionDecl;
        }

        List<String> arguments = new ArrayList<String>();
        while (!match(TokenType.RIGHT_PAREN)) {
            Token argument = advance();
            Token maybeComma = peek();
            if (maybeComma.type == TokenType.COMMA) {
                consume(TokenType.COMMA);
            } 
            arguments.add(argument.lexeme);
        }
        
        consume(TokenType.RIGHT_PAREN);
        Stmt functionDecl = new Stmt.FuncDecl(name, arguments, blockStatement());
        consume(TokenType.SEMICOLON);
        return functionDecl;
    }

    private Stmt statement() {
        Token maybePrint = peek();
        if (maybePrint.type == TokenType.PRINT) {
            advance();
            Expr expression = expression();
            Stmt statement = new Stmt.Print(expression);
            consume(TokenType.SEMICOLON);
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
        Expr maybeAssignmentToName = equality();

        Token maybeEquals = peek();
        if (maybeEquals.type == TokenType.EQUAL) {
            consume(TokenType.EQUAL);
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

    private Expr equality() {
        Expr leftHandSide = logicalAnd();
        while (match(TokenType.EQUAL_EQUAL)) {
            Token equalsEquals = advance();
            leftHandSide = new Expr.Binary(leftHandSide, equalsEquals, logicalAnd());
        }
        return leftHandSide;
    }

    private Expr logicalAnd() {
        Expr leftHandSide = comparison();
        while (match(TokenType.AND)) {
            Token logicalAndOperator = advance();
            leftHandSide = new Expr.Binary(leftHandSide, logicalAndOperator, comparison());
        }
        return leftHandSide;
    }

    private Expr comparison() {
        Expr leftHandSide = term();
        while (match(TokenType.GREATER, TokenType.LESS_EQUAL)) {
            Token comparisonOperator = advance();
            leftHandSide = new Expr.Binary(leftHandSide, comparisonOperator, term());
        }

        return leftHandSide;
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
        final Token maybeUnaryOperator = peek();
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
            if (match(TokenType.LEFT_PAREN)) {
                return new Expr.Func(first, functionArguments());
            } else {
                return new Expr.Variable(first);
            }
        }
    }

    private List<Expr> functionArguments() {
        List<Expr> arguments = new ArrayList<Expr>();
        consume(TokenType.LEFT_PAREN);
        while (!match(TokenType.RIGHT_PAREN)) {
            arguments.add(expression());
            Token maybeComma = peek();

            if (maybeComma.type == TokenType.COMMA) {
                consume(TokenType.COMMA);
            }
        }
        consume(TokenType.RIGHT_PAREN);
        return arguments;
    }

    private Token peek() {
        return this.tokens.get(current);
    }

    private Token advance() {
        Token advanced = this.tokens.get(current++);
        return advanced;
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

    private void consume(TokenType tokenType) {
        Token found = advance();
        if (found.type != tokenType) {
            throw new ParseError(found, tokenType);
        }
    }

    private static class ParseError extends RuntimeException {

        private final Token found;
        private final TokenType expected;

        private ParseError(Token found, TokenType expected) {
            super(getErrorMessage(found, expected));

            this.found = found;
            this.expected = expected;
        }

        private static String getErrorMessage(Token found, TokenType expected) {
            String errorMessage = "Lox compile error: expected " + expected + " at line " + found.line + ", found "
                    + "\"" + found.lexeme + "\".";
            return errorMessage;
        }
    }

    public static interface Reporter {
        public void reportError(ParserErrorParams params);
    }

    public static class ParserErrorParams {
        private final ParseError error;

        public ParserErrorParams(ParseError e) {
            this.error = e;
        }

        public String getErrorMessage() {
            return this.error.getMessage();
        }
    }

    public static class SysOutReporter implements Reporter {
        @Override
        public void reportError(ParserErrorParams params) {
            System.out.println(params.getErrorMessage());
        }
    }
}
