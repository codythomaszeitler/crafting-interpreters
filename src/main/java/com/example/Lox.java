package com.example;

import java.util.List;
import java.util.Map;

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

        Resolver closureBinder = new Resolver();
        Map<Id, StaticResolutionBlock> blockIdToBindings = 
            closureBinder.parseStaticVariableBindings(statements);

        Interpreter interpreter = new Interpreter(reporter, blockIdToBindings);
        interpreter.interpret(statements);
    }
}
