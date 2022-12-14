package com.example;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

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

    private final Map<Id, StaticResolutionBlock> blockIdToBindings;

    private Id currentBlockId;

    public Interpreter() {
        this(new SysOutReporter());
    }

    public Interpreter(Interpreter.Reporter reporter) {
        this(reporter, new HashMap<Id, StaticResolutionBlock>());
    }

    public Interpreter(Interpreter.Reporter reporter, Map<Id, StaticResolutionBlock> blockIdToBindings) {
        this.reporter = reporter;
        this.blockIdToBindings = blockIdToBindings;
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
        reporter.report(new InterpreterReportParams(value.toString()));
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
        StaticResolutionBlock block = this.blockIdToBindings.get(this.currentBlockId);

        Object value = null;
        if (block != null) {
            Integer distance = block.getDistance(expr.getId());
            if (distance == null) {
                value = getGlobalEnv().get(expr.name.lexeme);
                return value;
            }

            Environment env = getParentEnv(distance);
            value = env.get(expr.name.lexeme);
            System.out.println(value);
        } else {
            value = this.environment.get(expr.name.lexeme);
            System.out.println(value);
        }
        return value;
    }

    private Environment getParentEnv(Integer distance) {
        if (distance == null) {
            return getGlobalEnv();
        }

        Environment current = this.environment;
        for (Integer i = 0; i < distance; i++) {
            current = current.enclosing;
        }
        return current;
    }

    private Environment getGlobalEnv() {
        Environment global = environment;
        while (global.enclosing != null) {
            global = global.enclosing;
        }
        return global;
    }

    @Override
    public Object visitVariableAssign(Assign expr) {
        Object value = evaluate(expr.rightHandSide);
        StaticResolutionBlock block = this.blockIdToBindings.get(this.currentBlockId);

        if (block != null) {
            Integer distance = block.getDistance(expr.getId());
            Environment env = getParentEnv(distance);
            env.assign(expr.name, value);
        } else {
            this.environment.assign(expr.name, value);
        }
        return null;
    }

    @Override
    public Void visitBlockStatement(Block block) {
        Environment previous = this.environment;
        this.environment = new Environment(this.environment);

        Id previousBlockId = this.currentBlockId;
        this.currentBlockId = block.getId();
        for (Stmt statement : block.statements) {
            execute(statement);
        }
        this.environment = previous;
        this.currentBlockId = previousBlockId;
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
        Map<String, Object> argumentValues = new HashMap<>();
        Stmt.FuncDecl functionDeclaration = (Stmt.FuncDecl) this.environment.get(expr.name.lexeme);
        for (int i = 0; i < functionDeclaration.arguments.size(); i++) {
            String name = functionDeclaration.arguments.get(i);
            Expr value = expr.arguments.get(i);

            argumentValues.put(name, evaluate(value));
        }

        Environment previous = this.environment;
        this.environment = new Environment(previous);

        Id previousBlockId = this.currentBlockId;
        this.currentBlockId = functionDeclaration.getId();

        for (String varName : argumentValues.keySet()) {
            Object value = argumentValues.get(varName);
            this.environment.define(varName, value);
        }

        try {
            execute(functionDeclaration.block);
        } catch (Interpreter.Return e) {
            return e.value;
        } finally {
            this.environment = previous;
            this.currentBlockId = previousBlockId;
        }
        return null;
    }

    @Override
    public Void visitReturnStatement(Stmt.Return statement) {
        Object value = evaluate(statement.expression);
        throw new Interpreter.Return(value);
    }

    public static interface Reporter {
        public void report(InterpreterReportParams params);
    }

    public static class InterpreterReportParams {
        private final String message;

        public InterpreterReportParams(String message) {
            this.message = message;
        }

        public String getMessage() {
            return this.message;
        }
    }

    public static class SysOutReporter implements Reporter {
        @Override
        public void report(InterpreterReportParams params) {
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
