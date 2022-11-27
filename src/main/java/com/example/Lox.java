package com.example;

import java.util.List;

public class Lox {

    // I am 100% sure we should probably add on error reporting before we do this
    public Lox( Parser.Reporter parserReporter, Interpreter.Reporter interpreterReporter) {

    }

    public void execute(String source) {
        Scanner scanner = new Scanner(source);
        List<Token> tokens = scanner.scanTokens();

        Parser parser = new Parser(tokens);
        List<Stmt> statements = parser.parse();

        Interpreter interpreter = new Interpreter();
        interpreter.interpret(statements);
    }
}
