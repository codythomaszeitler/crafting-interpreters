package com.example;

import static org.junit.Assert.assertEquals;

import java.util.List;
import java.util.Map;
import org.junit.Test;

public class ResolverTest {
    @Test
    public void itShouldParseStaticResolutionBlocks() {
        String source = "{var a = 10; { func foo() {print a;}; var a = 3; foo(); }}";

        Scanner scanner = new Scanner(source);
        Parser parser = new Parser(scanner.scanTokens());

        List<Stmt> ast = parser.parse();

        // It should 100% be by reference since they should be the same exact tree. 
        // 
        Resolver testObject = new Resolver();
        // Well is the stack really correct at this moment? 
        Map<Id, StaticResolutionBlock> stmtIdToBlock = testObject.parseStaticVariableBindings(ast);

        assertEquals(3, stmtIdToBlock.size());
    }
}
