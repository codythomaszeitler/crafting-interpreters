package com.example;

import java.util.ArrayList;
import java.util.List;

import com.example.Interpreter.InterpreterReportParams;

public class TestInterpreterReporter implements Interpreter.Reporter {

    private List<InterpreterReportParams> params;

    public TestInterpreterReporter() {
        this.params = new ArrayList<>();
    }

    @Override
    public void report(InterpreterReportParams params) {
        this.params.add(params);
    }

    public Boolean hasMessage(String message) {
        Boolean hasMessage = false;
        for (InterpreterReportParams params : this.params) {
            if (params.getMessage().equals(message)) {
                hasMessage = true;
                break;
            }
        }
        return hasMessage;
    }
}
