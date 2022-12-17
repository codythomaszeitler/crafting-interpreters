#ifndef CLOXSTRING_HEADER
#define CLOXSTRING_HEADER

#include "value.h"
#include "object.h"

typedef struct StringObj {
    Obj type;
    int length;
    char* chars;
} StringObj;

StringObj* asString(const char *);
Value wrapString(const char*);

void freeStringObj(StringObj*);

#endif