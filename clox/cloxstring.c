#include "cloxstring.h"
#include "object.h"

#include <string.h>
#include <stdlib.h>
#include "value.h"
#include <stdbool.h>

StringObj* asString(const char *characters)
{
    int length = strlen(characters);

    char *inHeap = malloc(length + 1);
    strcpy(inHeap, characters);
    inHeap[length] = '\0';

    StringObj *stringObj = malloc(sizeof(StringObj));
    Obj *casted = (Obj *)stringObj;
    casted->type = ObjString;
    stringObj->length = length;
    stringObj->chars = inHeap;
    return stringObj;
}

bool isStringObj(Value value) 
{
    if (!isObject(value)) {
        return false;
    }

    Obj* unwrapped = unwrapObject(value);
    return unwrapped->type == ObjString;
}

Value wrapString(const char *characters)
{
    return wrapObject((Obj*)asString(characters));
}

void freeStringObj(StringObj *stringObj)
{
    free(stringObj->chars);
    free(stringObj);
}