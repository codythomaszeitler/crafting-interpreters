package com.example;

import java.util.ArrayList;
import java.util.List;

import com.example.Parser.ParserErrorParams;

public class TestParserReporter implements Parser.Reporter {

    private final List<String> errorMessages;

    public TestParserReporter() {
        this.errorMessages = new ArrayList<>();
    }

    @Override
    public void reportError(ParserErrorParams params) {
        this.errorMessages.add(params.getErrorMessage());
    }

    public Boolean hasErrorMessage(String errorMessage) {
        return this.errorMessages.contains(errorMessage);
    }
}
