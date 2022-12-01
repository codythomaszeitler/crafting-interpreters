package com.example;

public class SysOutErrorReport implements ErrorReport {
    @Override
    public void report(int line, String where, String message) {
        System.err.println("[line " + line + "] Error " + where + ": " + message);
    }
}
