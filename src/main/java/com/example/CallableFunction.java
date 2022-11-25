package com.example;

import java.util.List;

public class CallableFunction {

    private final Stmt.FuncDecl functionDeclaration;

    public CallableFunction(Stmt.FuncDecl functionDeclaration) {
        this.functionDeclaration = functionDeclaration;
    }

    public Object call(Environment environment, List<Object> arguments) {
        for (Stmt statement : functionDeclaration.block.statements) {

        }
        return null;
    }
}
