package com.example;

import static org.junit.Assert.assertEquals;

import java.util.List;

import org.junit.Test;

public class ParserTest {
    //  do not do that
    @Test
    public void test1() {
        Scanner scanner = new Scanner("print 3 + 5;");
        Parser testObject = new Parser(scanner.scanTokens());

        List<Stmt> statements = testObject.parse();
        assertEquals(1, statements.size());
    }
}
