#include "memory.h"
#include "common.h"
#include "value.h"
#include "vm.h"
#include <stdlib.h>

void *reallocate(void *pointer, usize oldSize, usize newSize) {
    (void)oldSize;
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

void freeObject(Obj *object) {
    switch (object->type) {
    case OBJ_STRING: {
        ObjString *string = (ObjString *)object;
        FREE_ARRAY(char, string->chars, string->length + 1U);
        FREE(ObjString, object);
        break;
    }
    }
}

void freeObjects(void) {
    Obj *object = vm.objects;
    while (object != NULL) {
        Obj *next = object->next;
        freeObject(object);
        object = next;
    }
}
