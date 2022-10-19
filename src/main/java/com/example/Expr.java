package com.example;

public abstract class Expr {

    public abstract <R> R accept(Visitor<R> visitor);

    interface Visitor<R> {
        R visitBinaryExpression(Expr.Binary visitor);
        R visitUnaryExpression(Expr.Unary visitor);
        R visitGroupingExpression(Expr.Grouping visitor);
        R visitLiteralExpression(Expr.Literal visitor);
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
        public final Object expression;

        public Literal(Object expression) {
            this.expression = expression;
        }

        public <R> R accept(Visitor<R> visitor) {
            return visitor.visitLiteralExpression(this);
        }
    }
}
