package com.example;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Stack;

import com.example.Expr.Assign;
import com.example.Expr.Binary;
import com.example.Expr.Func;
import com.example.Expr.Grouping;
import com.example.Expr.Literal;
import com.example.Expr.Unary;
import com.example.Expr.Variable;
import com.example.Stmt.Block;
import com.example.Stmt.Expression;
import com.example.Stmt.FuncDecl;
import com.example.Stmt.If;
import com.example.Stmt.Print;
import com.example.Stmt.Return;
import com.example.Stmt.Var;

public class Resolver implements Stmt.Visitor<Void>, Expr.Visitor<Object> {
    
    private final LinkedHashMap<Id, StaticResolutionBlock> stmtIdToResolution;
    private List<StmtIdWithDecls> blocks;

    private static class StmtIdWithDecls {
        private final Id stmtId;
        private final Set<String> declarations;

        private StmtIdWithDecls(Id stmtId) {
            this.stmtId = stmtId;
            this.declarations = new HashSet<>();
        }

        private void addDecl(String declaration) {
            this.declarations.add(declaration);
        }

        private boolean hasDecl(String declaration) {
            return this.declarations.contains(declaration);
        }
    }

    public Resolver() {
        this.stmtIdToResolution = new LinkedHashMap<>();
        this.blocks = new ArrayList<StmtIdWithDecls>();
    }

    public Map<Id, StaticResolutionBlock> parseStaticVariableBindings(List<Stmt> ast) {
        for (Stmt stmt : ast) {
            resolve(stmt);
        }
        return stmtIdToResolution;
    }

    private void resolve(Expr expr) {
        expr.accept(this);
    }

    private void resolve(Stmt stmt) {
        stmt.accept(this);
    }

    @Override
    public Object visitBinaryExpression(Binary visitor) {
        resolve(visitor.left);
        resolve(visitor.right);
        return null;
    }

    @Override
    public Object visitUnaryExpression(Unary visitor) {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public Object visitGroupingExpression(Grouping visitor) {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public Object visitLiteralExpression(Literal visitor) {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public Object visitVariableExpr(Variable expr) {
        Integer binding = findBindingIfExists(expr.name.lexeme);
        StaticResolutionBlock block = getCurrentBindingBlock();
        block.putDistance(expr.getId(), binding);
        return null;
    }

    @Override
    public Object visitVariableAssign(Assign expr) {
        Integer binding = findBindingIfExists(expr.name.lexeme);
        StaticResolutionBlock block = getCurrentBindingBlock();
        block.putDistance(expr.getId(), binding);
        return null;
    }

    @Override
    public Object visitFunctionExpression(Func expr) {
        return null;
    }

    @Override
    public Void visitPrintStatement(Print statement) {
        resolve(statement.expression);
        return null;
    }

    @Override
    public Void visitExpressionStatement(Expression statement) {
        // Okay so we found this thing. 
        resolve(statement.expression);
        return null;
    }

    @Override
    public Void visitVarDeclaration(Var statement) {
        String varName = statement.name.lexeme;
        StmtIdWithDecls block = getCurrentDeclsBlock();
        if (block != null) {
            block.addDecl(varName);
        }
        return null;
    }

    private StmtIdWithDecls getCurrentDeclsBlock() {
        int lastElementIndex = this.blocks.size() - 1;
        StmtIdWithDecls block = null;
        if (lastElementIndex != -1) {
            block = this.blocks.get(lastElementIndex);
            return block;
        } else {
            return block;
        }
    }

    @Override
    public Void visitBlockStatement(Block block) {
        startScope(block.getId());
        for (Stmt stmt : block.statements) {
            resolve(stmt);
        }
        endScope();
        return null;
    }

    private void startScope(Id stmtId) {
        this.stmtIdToResolution.put(stmtId, new StaticResolutionBlock());
        this.blocks.add(new StmtIdWithDecls(stmtId));
    }

    private void endScope() {
        int lastElementIndex = this.blocks.size() - 1;
        this.blocks.remove(lastElementIndex);
    }

    private StaticResolutionBlock getCurrentBindingBlock() {
        int lastElementIndex = this.blocks.size() - 1;
        StmtIdWithDecls block = this.blocks.get(lastElementIndex);
        return this.stmtIdToResolution.get(block.stmtId);
    }

    private Integer findBindingIfExists(String name) {
        Integer numJumps = null;
        for (int i = this.blocks.size(); i > 0; i--) {
            StmtIdWithDecls block = this.blocks.get(i - 1);
            if (block.hasDecl(name)) {
                // There has to be a plus one.
                numJumps = (this.blocks.size() - i);
                break;
            }
        }
        
        return numJumps;
    }

    @Override
    public Void visitIfStatement(If statement) {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public Void visitFunctionDeclStatement(FuncDecl statement) {
        startScope(statement.getId());
        resolve(statement.block);
        endScope();
        return null;
    }

    @Override
    public Void visitReturnStatement(Return statement) {
        // TODO Auto-generated method stub
        return null;
    }
}
