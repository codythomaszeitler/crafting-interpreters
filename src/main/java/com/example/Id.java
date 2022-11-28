package com.example;

public final class Id {
    private final long identifier;

    private Id(long identifier) {
        this.identifier = identifier;
    }

    // We really should use a static map here. 
    public static Id get(long identifier) {
        return new Id(identifier);
    }

    @Override
    public boolean equals(Object o) {
        if (o == null) {
            return false;
        }
        if (!(o instanceof Id)) {
            return false;
        }

        Id casted = (Id) o;
        return casted.identifier == this.identifier;
    }

    @Override
    public int hashCode() {
        return Long.hashCode(this.identifier);
    }

    @Override
    public String toString() {
        return Long.toString(this.identifier);
    }
}
