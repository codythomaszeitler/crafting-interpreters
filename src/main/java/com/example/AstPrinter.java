package com.example;

import com.example.Expr.Assign;
import com.example.Expr.Binary;
import com.example.Expr.Grouping;
import com.example.Expr.Literal;
import com.example.Expr.Unary;
import com.example.Expr.Variable;

public class AstPrinter implements Expr.Visitor<String> {
    public String print(Expr expression) {
        return expression.accept(this);
    }

    @Override
    public String visitBinaryExpression(Binary expression) {
        return parenthesize(expression.operator.lexeme, expression.left, expression.right);
    }

    @Override
    public String visitUnaryExpression(Unary expression) {
        return parenthesize(expression.operation.lexeme, expression.expression);
    }

    @Override
    public String visitGroupingExpression(Grouping expression) {
        return parenthesize("group", expression.expression);
    }

    @Override
    public String visitLiteralExpression(Literal expression) {
        if (expression.value == null) {
            return "nil";
        }
        return expression.value.toString();
    }

    private String parenthesize(String name, Expr... exprs) {
        StringBuilder builder = new StringBuilder();

        builder.append("(").append(name);
        for (Expr expression : exprs) {
            builder.append(" ");
            builder.append(expression.accept(this));
        }
        builder.append(")");
        return builder.toString();
    }

    @Override
    public String visitVariableExpr(Variable expr) {
        return null;
    }

    @Override
    public String visitVariableAssign(Assign expr) {
        return null;
    }
}
