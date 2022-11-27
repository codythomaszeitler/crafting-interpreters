package com.example;

import com.example.Interpreter.InterpreterReportParams;
import com.example.Parser.ParserErrorParams;
import com.example.Scanner.ScannerErrorParams;

public class SysOutReporter implements ErrorReporter {

    @Override
    public void report(InterpreterReportParams params) {
        System.out.println(params.getMessage());        
    }

    @Override
    public void reportError(ParserErrorParams params) {
        System.out.println(params.getErrorMessage());        
    }

    @Override
    public void reportError(ScannerErrorParams params) {
        System.out.println(params.getErrorMessage());        
    }
}
