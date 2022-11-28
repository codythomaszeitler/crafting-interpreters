package com.example;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.util.List;
import java.util.Map;

import org.junit.Test;

import com.example.Parser.ParserErrorParams;

public class InterpreterTest {

    private TestInterpreterReporter runInterpreterAgainst(String source) {
        Scanner scanner = new Scanner(source);
        List<Token> tokens = scanner.scanTokens();
        Parser parser = new Parser(tokens, new FailOnCompileError());

        List<Stmt> statements = parser.parse();

        TestInterpreterReporter reporter = new TestInterpreterReporter();

        Resolver resolver = new Resolver();
        Map<Id, StaticResolutionBlock> blockIdToResolution = resolver.parseStaticVariableBindings(statements);

        Interpreter testObject = new Interpreter(reporter, blockIdToResolution);
        testObject.interpret(statements);

        return reporter;
    }

    @Test
    public void test1() {
        TestInterpreterReporter reporter = runInterpreterAgainst(
                "print 3 + 5; var cody = 5; print cody; cody = 6; print cody;");

        assertTrue(reporter.hasMessage("8.0"));
        assertTrue(reporter.hasMessage("5.0"));
        assertTrue(reporter.hasMessage("6.0"));
    }

    @Test
    public void testBlockScoping() {
        String source = "var cody = 5; { var cody = 6; print cody; } print cody;";
        TestInterpreterReporter reporter = runInterpreterAgainst(source);

        assertTrue(reporter.hasMessage("6.0"));
        assertTrue(reporter.hasMessage("5.0"));
    }

    @Test
    public void testDoubleBlockingScope() {
        String source = "var cody = 5; { { var cody = 8; print cody; }  var cody = 6; print cody; { var cody = 123; print cody;} } print cody;";
        TestInterpreterReporter reporter = runInterpreterAgainst(source);

        assertTrue(reporter.hasMessage("6.0"));
        assertTrue(reporter.hasMessage("8.0"));
        assertTrue(reporter.hasMessage("123.0"));
        assertTrue(reporter.hasMessage("5.0"));
    }

    @Test
    public void itShouldBeAbleToPrintTrueInAVar() {
        String source = "var cody = true; print cody;";
        TestInterpreterReporter reporter = runInterpreterAgainst(source);

        assertTrue(reporter.hasMessage("true"));
    }

    @Test
    public void testIfConditionalWorksInTrueCase() {
        String source = "var cody = true; if (cody) {print \"test\"; }";
        TestInterpreterReporter reporter = runInterpreterAgainst(source);
        assertTrue(reporter.hasMessage("test"));
    }

    @Test
    public void testIfConditionalWorksInFalseCase() {
        String source = "var cody = false; if (cody) {print \"test\"; }";
        TestInterpreterReporter reporter = runInterpreterAgainst(source);
        assertFalse(reporter.hasMessage("test"));
    }

    @Test
    public void itShouldBeAbleToUseConditionalWithRawTrue() {
        String source = "if (true) {print \"test\"; }";
        TestInterpreterReporter reporter = runInterpreterAgainst(source);
        assertTrue(reporter.hasMessage("test"));
    }

    @Test
    public void itShouldBeAbleToNotEnterOnUnaryTrueOperation() {
        String source = "if (!true) {print \"test\"; }";
        TestInterpreterReporter reporter = runInterpreterAgainst(source);
        assertFalse(reporter.hasMessage("test"));
    }

    @Test
    public void itShouldBeAbleToRunEquals() {
        String source = "if (5 == 5) {print \"test\"; }";
        TestInterpreterReporter reporter = runInterpreterAgainst(source);
        assertTrue(reporter.hasMessage("test"));
    }

    @Test
    public void itShouldBeAbleToRunEqualsWhenFalse() {
        String source = "if (5 == 6) {print \"test\"; }";
        TestInterpreterReporter reporter = runInterpreterAgainst(source);
        assertFalse(reporter.hasMessage("test"));
    }

    @Test
    public void itShouldBeAbleToRunEqualsWithVars() {
        String source = "var cody = 5; var kirk = 5; if (cody == kirk) {print \"test\"; }";
        TestInterpreterReporter reporter = runInterpreterAgainst(source);
        assertTrue(reporter.hasMessage("test"));
    }

    @Test
    public void itShouldBeAbleToRunEqualsWithInlineAddition() {
        String source = "var cody = 5; var kirk = 4; if (cody == kirk + 1) {print \"test\"; }";
        TestInterpreterReporter reporter = runInterpreterAgainst(source);
        assertTrue(reporter.hasMessage("test"));
    }

    @Test
    public void itShouldBeAbleToDoMultiLevelEquals() {
        String source = "if (5 == 5 == 5 == 5 == 5 == 6) {print \"test\"; }";
        TestInterpreterReporter reporter = runInterpreterAgainst(source);
        assertFalse(reporter.hasMessage("test"));
    }

    @Test
    public void itShouldBeAbleToDoGreaterThan() {
        String source = "if ( 6 > 5 ) { print \"test\"; }";
        TestInterpreterReporter reporter = runInterpreterAgainst(source);
        assertTrue(reporter.hasMessage("test"));
    }

    @Test
    public void itShouldBeAbleToDoMultiLevelGreaterThan() {
        String source = "if ( 6 > 5 && 7 > 3 ) { print \"test\"; }";
        TestInterpreterReporter reporter = runInterpreterAgainst(source);
        assertTrue(reporter.hasMessage("test"));
    }

    @Test
    public void itShouldCallANamedFunction() {
        String source = "func add(a, b) { print a + b; }; add(2, 3);";
        TestInterpreterReporter reporter = runInterpreterAgainst(source);
        assertTrue(reporter.hasMessage("5.0"));
    }

    @Test
    public void itShouldPrintReturnValueFromFunction() {
        String source = "func add(a, b) { return a + b; }; var b = add(2, 3); print b;";
        TestInterpreterReporter reporter = runInterpreterAgainst(source);
        assertTrue(reporter.hasMessage("5.0"));
    }

    @Test
    public void itShouldDoFibNumbers() {
        String source = "func fib(n) { if (n <= 1) {return n;} return fib (n - 2) + fib(n - 1);}; print fib(9);";
        TestInterpreterReporter reporter = runInterpreterAgainst(source);
        assertTrue(reporter.hasMessage("34.0"));
    }

    @Test
    public void itShouldDoFibNumbersBaseCaseZero() {
        String source = "func fib(n) { if (n <= 1) {return n;} return fib (n - 2) + fib(n - 1);}; print fib(0);";
        TestInterpreterReporter reporter = runInterpreterAgainst(source);
        assertTrue(reporter.hasMessage("0.0"));
    }

    @Test
    public void itShouldDoFibNumbersBaseCaseOne() {
        String source = "func fib(n) { if (n <= 1) {return n;} return fib (n - 2) + fib(n - 1);}; print fib(1);";
        TestInterpreterReporter reporter = runInterpreterAgainst(source);
        assertTrue(reporter.hasMessage("1.0"));
    }

    @Test
    public void itShouldDoFibNumbersOneRecursion() {
        String source = "func fib(n) { if (n <= 1) {return n;} return fib (n - 2) + fib(n - 1);}; print fib(2);";
        TestInterpreterReporter reporter = runInterpreterAgainst(source);
        assertTrue(reporter.hasMessage("1.0"));
    }

    @Test
    public void itShouldRunAPrivateInnerFunction() {
        String source = "func foo() { func test() { print \"test\"; }; test();}; foo();";
        TestInterpreterReporter reporter = runInterpreterAgainst(source);
        assertTrue(reporter.hasMessage("test"));
    }

    private static class FailOnCompileError implements Parser.Reporter {
        @Override
        public void reportError(ParserErrorParams params) {
            throw new RuntimeException(params.getErrorMessage());
        }
    }
}
