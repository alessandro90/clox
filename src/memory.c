#include "memory.h"

#include "chunk.h"
#include "common.h"
#include "compiler.h"
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

void markObject(Obj *object) {
    if (object == NULL) { return; }
    if (object->isMarked) { return; }
#ifdef DEBUG_LOG_GC
    printf("%p mark ", (void *)object);
    printValue(OBJ_VAL(object));
    printf("\n");
#endif
    object->isMarked = true;

    if (vm.grayCapacity < vm.grayCount + 1U) {
        vm.grayCapacity = GROW_CAPACITY(vm.grayCapacity);
        Obj **newGrayStack = (Obj **)realloc(vm.grayStack, sizeof(Obj *) * vm.grayCapacity);
        if (newGrayStack == NULL) { exit(1); }  // NOLINT
        vm.grayStack = newGrayStack;
    }

    vm.grayStack[vm.grayCount++] = object;
}

void markValue(Value value) {
    if (IS_OBJ(value)) { markObject(AS_OBJ(value)); }
}

static void markArray(ValueArray *array) {
    for (usize i = 0; i < array->count; ++i) {
        markValue(array->values[i]);
    }
}

static void blackenObject(Obj *object) {
#ifdef DEBUG_LOG_GC
    printf("%p blacken ", (void *)object);
    printValue(OBJ_VAL(object));
    printf("\n");
#endif

    switch (object->type) {
    case OBJ_NATIVE:
    case OBJ_STRING:
        break;
    case OBJ_UPVALUE:
        markValue(((ObjUpvalue *)object)->closed);
        break;
    case OBJ_FUNCTION: {
        ObjFunction *function = (ObjFunction *)object;
        markObject((Obj *)function->name);
        markArray(&function->chunk.constants);
        break;
    }
    case OBJ_CLOSURE: {
        ObjClosure *closure = (ObjClosure *)object;
        markObject((Obj *)closure->function);
        for (usize i = 0; i < closure->upvalueCount; ++i) {
            markObject((Obj *)closure->upvalues[i]);
        }
        break;
    }
    }
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

static void markRoots(void) {
    for (Value *slot = vm.stack; slot < vm.stackTop; ++slot) {
        markValue(*slot);
    }

    for (i32 i = 0; i < vm.frameCount; ++i) {
        markObject((Obj *)vm.frames[i].closure);
    }

    for (ObjUpvalue *upvalue = vm.openUpvalues; upvalue != NULL; upvalue = upvalue->next) {
        markObject((Obj *)upvalue);
    }


    markTable(&vm.globals);
    markCompilerRoots();
}

static void traceReferences(void) {
    while (vm.grayCount > 0) {
        Obj *object = vm.grayStack[--vm.grayCount];
        blackenObject(object);
    }
}

static void sweep(void) {
    Obj *previous = NULL;
    Obj *object = vm.objects;
    while (object != NULL) {
        if (object->isMarked) {
            object->isMarked = false;
            previous = object;
            object = object->next;
        } else {
            Obj *unreached = object;
            object = object->next;
            if (previous != NULL) {
                previous->next = object;
            } else {
                vm.objects = object;
            }
            freeObject(unreached);
        }
    }
}

void collectGarbage(void) {
#ifdef DEBUG_LOG_GC
    printf("-- gc begin\n");
#endif

    markRoots();
    traceReferences();
    sweep();

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
    free(vm.grayStack);
}
