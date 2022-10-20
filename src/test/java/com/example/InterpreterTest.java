package com.example;

import java.util.List;

import org.junit.Test;

public class InterpreterTest {

    @Test
    public void test1() {
        Scanner scanner = new Scanner("print 3 + 5;");
        Parser parser = new Parser(scanner.scanTokens());

        List<Stmt> statements = parser.parse();

        Interpreter testObject = new Interpreter();
        testObject.interpret(statements);
    }
}
