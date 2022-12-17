#include "cloxstring.h"
#include "object.h"

#include <string.h>
#include <stdlib.h>
#include "value.h"

StringObj* asString(const char *characters)
{
    int length = strlen(characters);

    char *inHeap = malloc(length + 1);
    strcpy(inHeap, characters);
    inHeap[length] = '\0';

    StringObj *stringObj = malloc(sizeof(StringObj));
    Obj *casted = (Obj *)stringObj;
    casted->type = 1;
    stringObj->length = length;
    stringObj->chars = inHeap;
    return stringObj;
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