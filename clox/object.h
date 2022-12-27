#ifndef OBJECT_HEADER
#define OBJECT_HEADER

typedef enum ObjType {
    ObjString,
    ObjFunction,
    ObjClosure
} ObjType;

typedef struct Obj {
    ObjType type;
} Obj;

#endif