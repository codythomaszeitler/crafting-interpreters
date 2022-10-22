package com.example;

import java.util.List;

import com.example.Expr.Assign;
import com.example.Expr.Binary;
import com.example.Expr.Grouping;
import com.example.Expr.Literal;
import com.example.Expr.Unary;
import com.example.Expr.Variable;
import com.example.Stmt.Block;
import com.example.Stmt.Expression;
import com.example.Stmt.If;
import com.example.Stmt.Print;
import com.example.Stmt.Var;

public class Interpreter implements Stmt.Visitor<Void>, Expr.Visitor<Object> {

    private Environment environment = new Environment();
    private final Interpreter.Reporter reporter;

    public Interpreter() {
        this(new SysOutReporter());
    }

    public Interpreter(Interpreter.Reporter reporter) {
        this.reporter = reporter;
    }

    @Override
    public Void visitPrintStatement(Print statement) {
        Object value = evaluate(statement.expression);
        reporter.report(new ReportParams(value.toString()));
        return null;
    }

    @Override
    public Void visitExpressionStatement(Expression statement) {
        evaluate(statement.expression);
        return null;
    }

    @Override
    public Void visitVarDeclaration(Var statement) {
        this.environment.define(statement.name.lexeme, evaluate(statement.rightHandSide));
        return null;
    }

    void interpret(List<Stmt> statements) {
        for (Stmt statement : statements) {
            execute(statement);
        }
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

    @Override
    public Object visitVariableExpr(Variable expr) {
        return this.environment.get(expr.name.lexeme);
    }

    @Override
    public Object visitVariableAssign(Assign expr) {
        Object value = evaluate(expr.rightHandSide);
        environment.assign(expr.name, value);
        return null;
    }

    public static interface Reporter {
        public void report(ReportParams params);
    }

    public static class ReportParams {
        public final String message;

        public ReportParams(String message) {
            this.message = message;
        }
    }

    public static class SysOutReporter implements Reporter {
        @Override
        public void report(ReportParams params) {
            System.out.println(params.message);
        }
    }

    @Override
    public Void visitBlockStatement(Block block) {
        Environment previous = this.environment;
        this.environment = new Environment(this.environment);
        for (Stmt statement : block.statements) {
            execute(statement);
        }
        this.environment = previous;
        return null;
    }

    @Override
    public Void visitIfStatement(If statement) {
        // We do not need to worry about the environment in this case....
        // the block statement should really do that for us.

        Boolean shouldRunBlock = (Boolean) evaluate(statement.expression);

        if (shouldRunBlock) {
            execute(statement.block);
        }
        
        return null;
    }
}
