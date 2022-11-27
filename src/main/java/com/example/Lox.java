package com.example;

import java.util.List;

public class Lox {

    private final Reporter reporter;

    public Lox(Reporter reporter) {
        this.reporter = reporter;
    }

    public void execute(String source) {
        Scanner scanner = new Scanner(source, reporter);
        List<Token> tokens = scanner.scanTokens();

        Parser parser = new Parser(tokens, reporter);
        List<Stmt> statements = parser.parse();

        Interpreter interpreter = new Interpreter(reporter);
        interpreter.interpret(statements);
    }
}
