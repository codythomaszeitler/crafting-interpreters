package com.example;

import org.junit.Test;

public class LoxTest {

    @Test
    public void itShouldPrintHelloWorld() {
        Lox testObject = new Lox();
        testObject.execute("print \"Hello, World!\";");
    }
}
