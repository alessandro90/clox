#ifndef CLOX_OBJECT_H
#define CLOX_OBJECT_H

#include "chunk.h"
#include "common.h"
#include "value.h"

#define OBJ_TYPE(value) (AS_OBJ(value)->type)

#define IS_CLOSURE(value) isObjType(value, OBJ_CLOSURE)

#define IS_FUNCTION(value) isObjType(value, OBJ_FUNCTION)

#define IS_STRING(value) isObjType(value, OBJ_STRING)

#define IS_NATIVE(value) isObjType(value, OBJ_NATIVE)

#define AS_CLOSURE(value) ((ObjClosure *)AS_OBJ(value))
#define AS_FUNCTION(value) ((ObjFunction *)AS_OBJ(value))
#define AS_STRING(value) ((ObjString *)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString *)AS_OBJ(value))->chars)
#define AS_NATIVE(value) (((ObjNative *)AS_OBJ(value))->function)


typedef enum {
    OBJ_STRING,
    OBJ_FUNCTION,
    OBJ_NATIVE,
    OBJ_CLOSURE,
    OBJ_UPVALUE,
} ObjType;

struct Obj {
    ObjType type;
    struct Obj *next;
};

typedef struct {
    Obj obj;
    usize arity;
    usize upvalueCount;
    Chunk chunk;
    ObjString *name;
} ObjFunction;

typedef Value (*NativeFn)(i32 argCount, Value *args);

typedef struct {
    Obj obj;
    NativeFn function;
} ObjNative;

struct ObjString {
    Obj obj;
    usize length;
    char *chars;
    u32 hash;
};

typedef struct ObjUpvalue {
    Obj obj;
    Value *location;
} ObjUpvalue;

typedef struct {
    Obj obj;
    ObjFunction *function;
    ObjUpvalue **upvalues;
    usize upvalueCount;
} ObjClosure;

ObjClosure *newClosure(ObjFunction *function);
ObjFunction *newFunction(void);

ObjNative *newNative(NativeFn function);

ObjString *takeString(char *chars, usize length);

ObjString *copyString(const char *chars, usize length);

ObjUpvalue *newUpvalue(Value *slot);

void printObject(Value value);

static inline bool isObjType(Value value, ObjType type) {
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#endif
