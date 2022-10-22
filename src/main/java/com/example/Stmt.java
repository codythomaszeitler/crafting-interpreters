package com.example;

import java.util.List;

public abstract class Stmt {

    abstract <R> R accept(Visitor<R> visitor);

    interface Visitor<R> {
        R visitPrintStatement(Stmt.Print statement);

        R visitExpressionStatement(Stmt.Expression statement);

        R visitVarDeclaration(Stmt.Var statement);

        R visitBlockStatement(Stmt.Block statement);
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
        // This thing needs an environment during the ruin time.
        public final List<Stmt> statements;

        public Block(List<Stmt> statements) {
            this.statements = statements;
        }

        @Override
        public <R> R accept(Visitor<R> visitor) {
            return visitor.visitBlockStatement(this);
        }
    }
}
