package com.example;

import java.util.HashMap;
import java.util.Map;

public class Environment {
    final Environment enclosing;
    private final Map<String, Object> values = new HashMap<>();

    public Environment() {
        this.enclosing = null;
    }

    public Environment(Environment enclosing) {
        this.enclosing = enclosing;
    }

    void define(String name, Object value) {
        this.values.put(name, value);
    }

    void assign(Token name, Object value) {
        if (values.containsKey(name.lexeme)) {
            values.put(name.lexeme, value);
            return;
        }
    }

    Object get(String name) {
        if (this.values.containsKey(name)) {
            return this.values.get(name);
        }

        if (this.enclosing != null) {
            return this.enclosing.get(name);
        }
        return null;
    }
}
