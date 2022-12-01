package com.example;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotEquals;

import org.junit.Test;

public class IdTest {

    @Test
    public void itShouldBeEqualsWhenSameId() {
        long sameId = 1234;

        Id a = Id.get(sameId);
        Id b = Id.get(sameId);

        assertEquals(a, b);
        assertEquals(a.hashCode(), b.hashCode());
    }

    @Test
    public void itShouldNotBeEqualsWhenDifferentId() {

        Id a = Id.get(1234);
        Id b = Id.get(4321);

        assertNotEquals(a, b);
    }

    @Test
    public void itShouldNotBeEqualsToNull() {
        Id a = Id.get(1234);

        assertNotEquals(a, null);
    }

}
