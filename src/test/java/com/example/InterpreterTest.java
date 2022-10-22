package com.example;

import static org.junit.Assert.assertTrue;

import java.util.ArrayList;
import java.util.List;

import org.junit.Before;
import org.junit.Test;

import com.example.Interpreter.ReportParams;

public class InterpreterTest {

    private CatchSysOutReporter runInterpreterAgainst(String source) {
        Scanner scanner = new Scanner(source);
        List<Token> tokens = scanner.scanTokens();
        Parser parser = new Parser(tokens);

        List<Stmt> statements = parser.parse();

        CatchSysOutReporter reporter = new CatchSysOutReporter();

        Interpreter testObject = new Interpreter(reporter);
        testObject.interpret(statements);

        return reporter;
    }

    @Test
    public void test1() {
        CatchSysOutReporter reporter = runInterpreterAgainst("print 3 + 5; var cody = 5; print cody; cody = 6; print cody;");

        assertTrue(reporter.hasMessage("8.0"));
        assertTrue(reporter.hasMessage("5.0"));
        assertTrue(reporter.hasMessage("6.0"));
    }

    @Test
    public void testBlockScoping() {
        String source = "var cody = 5; { var cody = 6; print cody; } print cody;";
        CatchSysOutReporter reporter = runInterpreterAgainst(source);

        assertTrue(reporter.hasMessage("6.0"));
        assertTrue(reporter.hasMessage("5.0"));
    }

    @Test
    public void testDoubleBlockingScope() {
        String source = "var cody = 5; { { var cody = 8; print cody; }  var cody = 6; print cody; { var cody = 123; print cody;} } print cody;";
        CatchSysOutReporter reporter = runInterpreterAgainst(source);

        assertTrue(reporter.hasMessage("6.0"));
        assertTrue(reporter.hasMessage("8.0"));
        assertTrue(reporter.hasMessage("123.0"));
        assertTrue(reporter.hasMessage("5.0"));
    }

    @Test
    public void itShouldBeAbleToPrintTrueInAVar() {
        String source = "var cody = true; print cody;";
        CatchSysOutReporter reporter = runInterpreterAgainst(source);

        assertTrue(reporter.hasMessage("true"));
    }

    @Test
    public void testIfConditionalWorks() {
        String source = "var cody = true; if (cody) {print \"test\"; }";
        CatchSysOutReporter reporter = runInterpreterAgainst(source);
        assertTrue(reporter.hasMessage("test"));
    }

    private static class CatchSysOutReporter implements Interpreter.Reporter {

        private List<ReportParams> params;

        public CatchSysOutReporter() {
            this.params = new ArrayList<>();
        }

        @Override
        public void report(ReportParams params) {
            this.params.add(params);
        }

        public Boolean hasMessage(String message) {
            Boolean hasMessage = false;
            for (ReportParams params : this.params) {
                if (params.message.equals(message)) {
                    hasMessage = true;
                    break;
                }
            }
            return hasMessage;
        }
    }
}
