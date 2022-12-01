package com.example;

import com.example.Interpreter.InterpreterReportParams;
import com.example.Parser.ParserErrorParams;
import com.example.Scanner.ScannerErrorParams;

public interface Reporter extends Scanner.Reporter, Parser.Reporter, Interpreter.Reporter {
    public void report(InterpreterReportParams params);

    public void reportError(ParserErrorParams params);

    public void reportError(ScannerErrorParams params);
}
