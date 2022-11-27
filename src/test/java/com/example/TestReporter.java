package com.example;

import java.util.ArrayList;
import java.util.List;

import com.example.Interpreter.InterpreterReportParams;
import com.example.Parser.ParserErrorParams;
import com.example.Scanner.ScannerErrorParams;

public class TestReporter implements Reporter {

    private final List<String> printMessages;

    public TestReporter() {
        this.printMessages = new ArrayList<>();
    }

    @Override
    public void report(InterpreterReportParams params) {
        this.printMessages.add(params.getMessage());
    }

    public Boolean hasPrintMessage(String printMessage) {
        return this.printMessages.contains(printMessage);
    }

    @Override
    public void reportError(ParserErrorParams params) {
        throw new IllegalArgumentException(params.getErrorMessage());
    }

    @Override
    public void reportError(ScannerErrorParams params) {
        throw new IllegalArgumentException(params.getErrorMessage());
    }
}
