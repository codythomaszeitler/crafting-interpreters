package com.example;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

import java.util.ArrayList;
import java.util.List;

import org.junit.Test;

import com.example.Parser.ErrorParams;

public class ParserTest {
    //  do not do that
    @Test
    public void test1() {
        Scanner scanner = new Scanner("print 3 + 5; var cody = 5; print cody;");
        List<Token> tokens = scanner.scanTokens();
        Parser testObject = new Parser(tokens);

        List<Stmt> statements = testObject.parse();
        assertEquals(3, statements.size());
    }

    // It's passing because there is a null in there making the list size 2.
    // That really, really should not work lmfao.
    @Test
    public void itShouldBeAbleToParserAnIfStatement() {
        String source = "var cody = false; if (cody) {print \"test\"; }";
        Scanner scanner = new Scanner(source);
        List<Token> tokens = scanner.scanTokens();

        Parser testObject = new Parser(tokens);

        List<Stmt> statements = testObject.parse();
        // So there should be two statements in here?
        assertEquals(2, statements.size());
    }

    @Test
    public void itShouldReportErrorIfIfStatementIsNotFollowedByParens() {
        String source = "if true) { print \"test\"; }";
        Scanner scanner = new Scanner(source);
        List<Token> tokens = scanner.scanTokens();

        CatchSysOutReporter reporter = new CatchSysOutReporter();

        Parser testObject = new Parser(tokens, reporter);
        testObject.parse();

        assertTrue(reporter.hasErrorMessage("Lox compile error: expected LEFT_PAREN at line 1, found \"true\"."));
    }

    @Test
    public void itShouldReportErrorIfRightParentMissingInIfStatement() {
        String source = "if (true { print \"test\"; }";
        Scanner scanner = new Scanner(source);
        List<Token> tokens = scanner.scanTokens();

        CatchSysOutReporter reporter = new CatchSysOutReporter();

        Parser testObject = new Parser(tokens, reporter);
        testObject.parse();

        assertTrue(reporter.hasErrorMessage("Lox compile error: expected RIGHT_PAREN at line 1, found \"{\"."));
    }

    private class CatchSysOutReporter implements Parser.Reporter {

        private final List<String> errorMessages;
        public CatchSysOutReporter() {
            this.errorMessages = new ArrayList<>();
        }

        @Override
        public void reportError(ErrorParams params) {
            this.errorMessages.add(params.getErrorMessage()); 
        }

        public Boolean hasErrorMessage(String errorMessage) {
            return this.errorMessages.contains(errorMessage);
        }
    }
}
