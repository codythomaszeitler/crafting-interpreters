package com.example;

import static org.junit.Assert.assertEquals;

import org.junit.Test;

public class ExprTest {
    
    @Test
    public void testPrintOutBinaryExpression() {

        // Okay so why is it appropriate to use a token here? 
        // Wait... an expression is not going to be found during lexical analysis,
        // it is going to be found in 

        Expr binary = new Expr.Binary(
            new Expr.Unary(
                new Token(TokenType.MINUS, "-", null, 1),
                new Expr.Literal(123)
            ),
            new Token(TokenType.STAR, "*", null, 1),
            new Expr.Grouping(
                new Expr.Literal(45.67)
            )
        );

        AstPrinter printer = new AstPrinter();
        String expected = "(* (- 123) (group 45.67))";

        String actual = printer.print(binary);
        assertEquals(expected, actual);
    }
}
