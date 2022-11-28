package com.example;

import java.util.HashMap;
import java.util.Map;

public class StaticResolutionBlock {

    private final Map<Id, Integer> expressionToHops;

    public StaticResolutionBlock() {
        this.expressionToHops = new HashMap<>();
    }

    public Integer getDistance(Id exprId) {
        return this.expressionToHops.get(exprId);
    }

    public void putDistance(Id exprId, Integer distance) {
        this.expressionToHops.put(exprId, distance);
    }
}
