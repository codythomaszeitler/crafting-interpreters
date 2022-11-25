package com.example;

import java.util.List;

import com.example.Expr.Assign;
import com.example.Expr.Binary;
import com.example.Expr.Func;
import com.example.Expr.Grouping;
import com.example.Expr.Literal;
import com.example.Expr.Unary;
import com.example.Expr.Variable;
import com.example.Stmt.Block;
import com.example.Stmt.Expression;
import com.example.Stmt.FuncDecl;
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

    @Override
    public Object visitBinaryExpression(Binary visitor) {
        if (TokenType.PLUS == visitor.operator.type) {
            Double left = (Double) evaluate(visitor.left);
            Double right = (Double) evaluate(visitor.right);
            return left + right;
        } else if (TokenType.EQUAL_EQUAL == visitor.operator.type) {
            Object left = evaluate(visitor.left);
            Object right = evaluate(visitor.right);
            return left.equals(right);
        } else if (TokenType.GREATER == visitor.operator.type) {
            Double left = (Double) evaluate(visitor.left);
            Double right = (Double) evaluate(visitor.right);
            return left > right;
        } else if (TokenType.AND == visitor.operator.type) {
            Boolean left = (Boolean) evaluate(visitor.left);
            Boolean right = (Boolean) evaluate(visitor.right);
            return left && right;
        } else if (TokenType.LESS_EQUAL == visitor.operator.type) {
            Double left = (Double) evaluate(visitor.left);
            Double right = (Double) evaluate(visitor.right);
            return left <= right;
        } 
        else if (TokenType.MINUS == visitor.operator.type) {
            Double left = (Double) evaluate(visitor.left);
            Double right = (Double) evaluate(visitor.right);
            return left - right;
        } else {
            return null;
        }
    }

    @Override
    public Object visitUnaryExpression(Unary visitor) {
        if (TokenType.BANG == visitor.operation.type) {
            Boolean resolved = (Boolean) evaluate(visitor.expression);
            Boolean banged = !resolved;
            return banged;
        }
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
        Boolean shouldRunBlock = (Boolean) evaluate(statement.expression);
        if (shouldRunBlock) {
            execute(statement.block);
        }
        return null;
    }

    @Override
    public Void visitFunctionDeclStatement(FuncDecl statement) {
        this.environment.define(statement.name.lexeme, statement);
        return null;
    }

    @Override
    public Object visitFunctionExpression(Func expr) {
        Environment previous = this.environment;
        this.environment = new Environment(previous);

        Stmt.FuncDecl functionDeclaration = (Stmt.FuncDecl) previous.get(expr.name.lexeme);
        for (int i = 0; i < functionDeclaration.arguments.size(); i++) {
            String name = functionDeclaration.arguments.get(i);
            Expr value = expr.arguments.get(i);

            this.environment.define(name, evaluate(value));
        }

        try {
            execute(functionDeclaration.block);
        } catch (Interpreter.Return e) {
            return e.value;
        } finally {
            this.environment = previous;
        }
        return null;
    }

    @Override
    public Void visitReturnStatement(Stmt.Return statement) {
        Object value = evaluate(statement.expression);
        throw new Interpreter.Return(value);
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

    private static class Return extends RuntimeException {
        public final Object value;

        public Return(Object value) {
            this.value = value;
        }
    }
}
