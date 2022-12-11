#ifndef CLOXSTRING_HEADER
#define CLOXSTRING_HEADER

#include "object.h"

typedef struct StringObj {
    Obj type;
    int length;
    char* chars;
} StringObj;

StringObj* wrapString(const char*);

void freeStringObj(StringObj*);

#endif