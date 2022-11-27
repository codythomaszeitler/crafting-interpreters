package com.example;

import static org.junit.Assert.assertTrue;

import org.junit.Test;

public class LoxTest {
    private final Lox testObject;

    private final TestReporter reporter;

    public LoxTest() {
        this.reporter = new TestReporter();
        this.testObject = new Lox(reporter);
    }

    @Test
    public void itShouldPrintHelloWorld() {
        testObject.execute("print \"Hello, World!\";");
        assertTrue(this.reporter.hasPrintMessage("Hello, World!"));
    }
}
