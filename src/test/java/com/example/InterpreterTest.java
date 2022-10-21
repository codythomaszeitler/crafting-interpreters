package com.example;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

import java.util.ArrayList;
import java.util.List;

import org.junit.Test;

import com.example.Interpreter.ReportParams;

public class InterpreterTest {

    @Test
    public void test1() {
        Scanner scanner = new Scanner("print 3 + 5; var cody = 5; print cody; cody = 6; print cody;");
        List<Token> tokens = scanner.scanTokens();
        Parser parser = new Parser(tokens);

        List<Stmt> statements = parser.parse();

        CatchSysOutReporter reporter = new CatchSysOutReporter();

        Interpreter testObject = new Interpreter(reporter);
        testObject.interpret(statements);

        assertTrue(reporter.hasMessage("8.0"));
        assertTrue(reporter.hasMessage("5.0"));
        assertTrue(reporter.hasMessage("6.0"));
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
