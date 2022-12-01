package com.example;

import java.util.ArrayList;
import java.util.List;

import com.example.Scanner.ScannerErrorParams;

public class TestScannerReporter implements Scanner.Reporter{

    private final List<String> errorMessages;

    public TestScannerReporter() {
        this.errorMessages = new ArrayList<>();
    }

    @Override
    public void reportError(ScannerErrorParams params) {
        this.errorMessages.add(params.getErrorMessage()); 
    }

    public Boolean hasErrorMessage(String errorMessage) {
        return this.errorMessages.contains(errorMessage);
    }
}
