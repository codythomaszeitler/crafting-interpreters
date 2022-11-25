package com.example;

import java.util.ArrayList;
import java.util.List;

public abstract class Stmt {

    abstract <R> R accept(Visitor<R> visitor);

    interface Visitor<R> {
        R visitPrintStatement(Stmt.Print statement);

        R visitExpressionStatement(Stmt.Expression statement);

        R visitVarDeclaration(Stmt.Var statement);

        R visitBlockStatement(Stmt.Block statement);

        R visitIfStatement(Stmt.If statement);

        R visitFunctionDeclStatement(Stmt.FuncDecl statement);

        R visitReturnStatement(Stmt.Return statement);
    }

    static class Print extends Stmt {
        Print(Expr expression) {
            this.expression = expression;
        }

        @Override
        <R> R accept(Visitor<R> visitor) {
            return visitor.visitPrintStatement(this);
        }

        final Expr expression;
    }

    static class Expression extends Stmt {
        Expression(Expr expression) {
            this.expression = expression;
        }

        @Override
        <R> R accept(Visitor<R> visitor) {
            return visitor.visitExpressionStatement(this);
        }

        final Expr expression;
    }

    public static class Var extends Stmt {

        public final Token name;
        public final Expr rightHandSide;

        public Var(Token name, Expr rightHandSide) {
            this.name = name;
            this.rightHandSide = rightHandSide;
        }

        @Override
        <R> R accept(Visitor<R> visitor) {
            return visitor.visitVarDeclaration(this);
        }
    }

    public static class Block extends Stmt {
        public final List<Stmt> statements;

        public Block(List<Stmt> statements) {
            this.statements = statements;
        }

        @Override
        public <R> R accept(Visitor<R> visitor) {
            return visitor.visitBlockStatement(this);
        }
    }
   
    public static class If extends Stmt {

        public final Expr expression;
        public final Block block;

        public If(Expr expression, Stmt.Block block) {
            this.expression = expression;
            this.block = block;
        }

        @Override
        public <R> R accept(Visitor<R> visitor) {
            return visitor.visitIfStatement(this);
        }

    }

    public static class FuncDecl extends Stmt {

        public final Token name;
        public final List<String> arguments;
        public final Stmt.Block block;

        public FuncDecl(Token name, List<String> arguments, Stmt.Block block) {
            this.name = name;
            this.arguments = new ArrayList<String>(arguments);
            this.block = block;
        }

        @Override
        public <R> R accept(Visitor<R> visitor) {
            return visitor.visitFunctionDeclStatement(this);
        }
    }

    public static class Return extends Stmt {
        public final Expr expression;

        public Return(Expr expression) {
            this.expression = expression;
        }

        @Override
        <R> R accept(Visitor<R> visitor) {
            return visitor.visitReturnStatement(this);
        }
    }
}
