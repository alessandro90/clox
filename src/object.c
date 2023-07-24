#include "object.h"
#include "chunk.h"
#include "memory.h"
#include "string.h"
#include "table.h"
#include "value.h"
#include "vm.h"
#include <stdio.h>

#define ALLOCATE_OBJ(type, objectType) \
    (type *)allocateObject(sizeof(type), objectType)

static Obj *allocateObject(usize size, ObjType type) {
    Obj *object = (Obj *)reallocate(NULL, 0, size);
    object->type = type;
    object->isMarked = false;
    object->next = vm.objects;
    vm.objects = object;
    return object;
}

ObjClass *newClass(ObjString *name) {
    ObjClass *klass = ALLOCATE_OBJ(ObjClass, OBJ_CLASS);
    klass->name = name;
    return klass;
}

ObjInstance *newInstance(ObjClass *klass) {
    ObjInstance *instance = ALLOCATE_OBJ(ObjInstance, OBJ_INSTANCE);
    instance->klass = klass;
    initTable(&instance->fields);
    return instance;
}

ObjClosure *newClosure(ObjFunction *function) {
    ObjUpvalue **upvalues = ALLOCATE(ObjUpvalue *, function->upvalueCount);
    for (usize i = 0; i < function->upvalueCount; ++i) {
        upvalues[i] = NULL;
    }
    ObjClosure *closure = ALLOCATE_OBJ(ObjClosure, OBJ_CLOSURE);
    closure->function = function;
    closure->upvalues = upvalues;
    closure->upvalueCount = function->upvalueCount;
    return closure;
}

ObjFunction *newFunction(void) {
    ObjFunction *function = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION);
    function->arity = 0;
    function->name = NULL;
    function->upvalueCount = 0;
    initChunk(&function->chunk);
    return function;
}

ObjNative *newNative(NativeFn function) {
    ObjNative *native = ALLOCATE_OBJ(ObjNative, OBJ_NATIVE);
    native->function = function;
    return native;
}

static ObjString *allocateString(char *chars, usize length, u32 hash) {
    ObjString *string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    string->length = length;
    string->chars = chars;
    string->hash = hash;
    push(OBJ_VAL(string));
    tableSet(&vm.strings, string, NIL_VAL);
    pop();
    return string;
}

static u32 hashString(const char *key, usize length) {
    u64 hash = 2166136261U;  // NOLINT
    for (usize i = 0; i < length; ++i) {
        hash ^= (u64)key[i];
        hash *= 16777619U;  // NOLINT
    }
    return (u32)hash;
}

ObjString *takeString(char *chars, usize length) {
    u32 const hash = hashString(chars, length);

    ObjString *interned = tableFindString(&vm.strings, chars, length, hash);
    if (interned != NULL) {
        FREE_ARRAY(char, chars, length + 1U);
        return interned;
    }
    return allocateString(chars, length, hash);
}

ObjString *copyString(const char *chars, usize length) {
    char *heapChars = ALLOCATE(char, length + 1U);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';
    u32 const hash = hashString(chars, length);
    ObjString *interned = tableFindString(&vm.strings, chars, length, hash);
    if (interned != NULL) { return interned; }
    return allocateString(heapChars, length, hash);
}

ObjUpvalue *newUpvalue(Value *slot) {
    ObjUpvalue *upvalue = ALLOCATE_OBJ(ObjUpvalue, OBJ_UPVALUE);
    upvalue->location = slot;
    upvalue->next = NULL;
    upvalue->closed = NIL_VAL;
    return upvalue;
}

static void printFunction(ObjFunction *function) {
    if (function->name == NULL) {
        printf("<script>");
        return;
    }
    printf("<fn %s>", function->name->chars);
}

void printObject(Value value) {
    switch (OBJ_TYPE(value)) {
    case OBJ_STRING:
        printf("%s", AS_CSTRING(value));
        break;
    case OBJ_FUNCTION:
        printFunction(AS_FUNCTION(value));
        break;
    case OBJ_NATIVE:
        printf("<native fn>");
        break;
    case OBJ_CLOSURE:
        printFunction(AS_CLOSURE(value)->function);
        break;
    case OBJ_UPVALUE:
        printf("upvalue");
        break;
    case OBJ_CLASS:
        printf("%s", AS_CLASS(value)->name->chars);
        break;
    case OBJ_INSTANCE:
        printf("%s instance", AS_INSTANCE(value)->klass->name->chars);
        break;
    }
}
