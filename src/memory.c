#include "memory.h"
#include "chunk.h"
#include "common.h"
#include "object.h"
#include "value.h"
#include "vm.h"
#include <stdlib.h>
#ifdef DEBUG_LOG_GC
#include "debug.h"
#include <stdio.h>
#endif

void *reallocate(void *pointer, usize oldSize, usize newSize) {
    if (newSize > oldSize) {
#ifdef DEBUG_STRESS_GC
        collectGarbage();
#endif
    }
    if (newSize == 0) {
        free(pointer);
        return NULL;
    }
    void *result = realloc(pointer, newSize);
    if (result == NULL) {
        exit(1);  // NOLINT
    }
    return result;
}

static void freeObject(Obj *object) {
#ifdef DEBUG_LOG_GC
    printf("%p free type %ul\n", (void *)object, object->type);
#endif
    switch (object->type) {
    case OBJ_STRING: {
        ObjString *string = (ObjString *)object;
        FREE_ARRAY(char, string->chars, string->length + 1U);
        FREE(ObjString, object);
        break;
    }
    case OBJ_FUNCTION: {
        ObjFunction *function = (ObjFunction *)object;
        freeChunk(&function->chunk);
        FREE(ObjFunction, object);
        break;
    }
    case OBJ_NATIVE:
        FREE(ObjNative, object);
        break;
    case OBJ_CLOSURE: {
        ObjClosure *closure = (ObjClosure *)object;
        FREE_ARRAY(ObjUpvalue *, closure->upvalues, closure->upvalueCount);
        FREE(ObjClosure, object);
    } break;
    case OBJ_UPVALUE:
        FREE(ObjUpvalue, object);
        break;
    }
}

void collectGarbage(void) {
#ifdef DEBUG_LOG_GC
    printf("-- gc begin\n");
#endif

#ifdef DEBUG_LOG_GC
    printf("-- gc end\n");
#endif
}

void freeObjects(void) {
    Obj *object = vm.objects;
    while (object != NULL) {
        Obj *next = object->next;
        freeObject(object);
        object = next;
    }
}
