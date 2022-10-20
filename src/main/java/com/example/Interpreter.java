package com.example;

import java.util.List;

import com.example.Expr.Binary;
import com.example.Expr.Grouping;
import com.example.Expr.Literal;
import com.example.Expr.Unary;
import com.example.Stmt.Expression;
import com.example.Stmt.Print;

public class Interpreter implements Stmt.Visitor<Void>, Expr.Visitor<Object> {

    @Override
    public Void visitPrintStatement(Print statement) {
        Double value = (Double)evaluate(statement.expression);
        System.out.println(value);
        return null;
    }

    @Override
    public Void visitExpressionStatement(Expression statement) {
        evaluate(statement.expression);
        return null;
    }

    void interpret(List<Stmt> statements) {
        // try {
        for (Stmt statement : statements) {
            execute(statement);
        }
        // } catch (RuntimeException error) {
        // System.out.print(error.getMessage());
        // }
    }

    private void execute(Stmt statement) {
        statement.accept(this);
    }

    private Object evaluate(Expr expression) {
        return expression.accept(this);
    }

    @Override
    public Object visitBinaryExpression(Binary visitor) {
        Double left = (Double) evaluate(visitor.left);
        Double right = (Double) evaluate(visitor.right);
        return left + right;
    }

    @Override
    public Object visitUnaryExpression(Unary visitor) {
        return null;
    }

    @Override
    public Object visitGroupingExpression(Grouping visitor) {
        return null;
    }

    @Override
    public Object visitLiteralExpression(Literal visitor) {
        return visitor.value;
    }
}
