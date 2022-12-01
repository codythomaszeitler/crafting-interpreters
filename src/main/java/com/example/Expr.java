package com.example;

import java.util.ArrayList;
import java.util.List;

public abstract class Expr {

    private static long currentId = -1;

    private final Id id;

    public Expr() {
        this.id = Id.get(currentId--);
    }

    public Id getId() {
        return this.id;
    }

    public abstract <R> R accept(Visitor<R> visitor);

    interface Visitor<R> {
        R visitBinaryExpression(Expr.Binary visitor);

        R visitUnaryExpression(Expr.Unary visitor);

        R visitGroupingExpression(Expr.Grouping visitor);

        R visitLiteralExpression(Expr.Literal visitor);

        R visitVariableExpr(Expr.Variable expr);

        R visitVariableAssign(Expr.Assign expr);

        R visitFunctionExpression(Expr.Func expr);
    }

    public static class Binary extends Expr {

        public final Expr left;
        public final Token operator;
        public final Expr right;

        public Binary(Expr left, Token operator, Expr right) {
            this.left = left;
            this.operator = operator;
            this.right = right;
        }

        public <R> R accept(Visitor<R> visitor) {
            return visitor.visitBinaryExpression(this);
        }
    }

    public static class Grouping extends Expr {

        public final Expr expression;

        public Grouping(Expr expression) {
            this.expression = expression;
        }

        public <R> R accept(Visitor<R> visitor) {
            return visitor.visitGroupingExpression(this);
        }
    }

    public static class Unary extends Expr {

        public final Token operation;
        public final Expr expression;

        public Unary(Token operation, Expr expression) {
            this.operation = operation;
            this.expression = expression;
        }

        public <R> R accept(Visitor<R> visitor) {
            return visitor.visitUnaryExpression(this);
        }
    }

    public static class Literal extends Expr {
        public final Object value;

        public Literal(Object value) {
            this.value = value;
        }

        public <R> R accept(Visitor<R> visitor) {
            return visitor.visitLiteralExpression(this);
        }
    }

    public static class Assign extends Expr {

        public final Token name;
        public final Expr rightHandSide;

        public Assign(Token name, Expr rightHandSide) {
            this.name = name;
            this.rightHandSide = rightHandSide;
        }

        @Override
        public <R> R accept(Visitor<R> visitor) {
            return visitor.visitVariableAssign(this);
        }
    }

    static class Variable extends Expr {
        Variable(Token name) {
            this.name = name;
        }

        @Override
        public <R> R accept(Visitor<R> visitor) {
            return visitor.visitVariableExpr(this);
        }

        final Token name;
    }

    public static class Func extends Expr {
        public final Token name;
        public final List<Expr> arguments;

        public Func(Token name, List<Expr> arguments) {
            this.name = name;
            this.arguments = new ArrayList<Expr>(arguments);
        }

        @Override
        public <R> R accept(Visitor<R> visitor) {
            return visitor.visitFunctionExpression(this);
        }
    }
}
