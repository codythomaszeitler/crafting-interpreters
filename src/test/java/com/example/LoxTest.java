package com.example;

import org.junit.Test;

public class LoxTest {
    private final Lox testObject; 
    private final TestInterpreterReporter interpreterReporter;
    private final TestParserReporter parserReporter;

    public LoxTest(){
        this.interpreterReporter = new TestInterpreterReporter();
        this.parserReporter = new TestParserReporter();
        this.testObject = new Lox(parserReporter, interpreterReporter);
    }

    @Test
    public void itShouldPrintHelloWorld() {
        testObject.execute("print \"Hello, World!\";");
    }
}
