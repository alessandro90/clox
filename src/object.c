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
    object->next = vm.objects;
    vm.objects = object;
    return object;
}

ObjFunction *newFunction(void) {
    ObjFunction *function = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION);
    function->arity = 0;
    function->name = NULL;
    initChunk(&function->chunk);
    return function;
}

static ObjString *allocateString(char *chars, usize length, u32 hash) {
    ObjString *string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    string->length = length;
    string->chars = chars;
    string->hash = hash;
    tableSet(&vm.strings, string, NIL_VAL);
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

static void printFunction(ObjFunction *function) {
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
    }
}
