package com.example;

import java.util.ArrayList;
import java.util.List;

import com.example.Interpreter.ReportParams;

public class TestInterpreterReporter implements Interpreter.Reporter {

    private List<ReportParams> params;

    public TestInterpreterReporter() {
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
