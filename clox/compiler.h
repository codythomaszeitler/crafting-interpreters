#ifndef COMPILER_HEADER
#define COMPILER_HEADER

#include "chunk.h"
#include "vm.h"
#include "functionobj.h"
#include "scanner.h"

typedef enum {
  PREC_NONE,
  PREC_ASSIGNMENT,  // =
  PREC_OR,          // or
  PREC_AND,         // and
  PREC_EQUALITY,    // == !=
  PREC_COMPARISON,  // < > <= >=
  PREC_TERM,        // + -
  PREC_FACTOR,      // * /
  PREC_UNARY,       // ! -
  PREC_CALL,        // . ()
  PREC_PRIMARY
} Precedence;

typedef struct Interpreter {
    void (*onStdOut) (char*);
    VirtualMachine vm;
} Interpreter;

void initInterpreter(Interpreter*);
void freeInterpreter(Interpreter*);

void runInterpreter(Interpreter*, const char* sourceCode);
void compile(FunctionObj* functionObj, TokenArrayIterator* tokens);
TokenArrayIterator tokenize(const char* sourceCode);
#endif