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

    @Test
    public void itShouldDoAClosure() {
        String source = "{var a = 10; { func foo() {print a;}; var a = 3; foo(); }}";
        testObject.execute(source);
        assertTrue(this.reporter.hasPrintMessage("10.0"));
    }

    @Test
    public void itShouldClosureAgainstAGlobalValue() {
        String source = "var a = 10; { func foo() {print a;}; var a = 3; foo(); }";
        testObject.execute(source);
        assertTrue(this.reporter.hasPrintMessage("10.0"));
    }

    @Test
    public void itShouldClosureInAddition() {
        String source = "{var a = 10; var b = 10; {func add() {print a + b;}; add(); a = 3; b = 3;}}";
        testObject.execute(source);
        assertTrue(this.reporter.hasPrintMessage("20.0"));
    }
}
