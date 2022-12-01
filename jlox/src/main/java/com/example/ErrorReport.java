package com.example;

public interface ErrorReport {
    public void report(int line, String where, String message);
}