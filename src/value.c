#include "value.h"
#include "common.h"
#include "memory.h"
#include "object.h"
#include <stdio.h>
#include <string.h>

void initValueArray(ValueArray *array) {
    array->count = 0;
    array->capacity = 0;
    array->values = NULL;
}

void writeValueArray(ValueArray *array, Value value) {
    if (array->capacity < array->count + 1U) {
        usize oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(oldCapacity);
        array->values = GROW_ARRAY(Value, array->values, oldCapacity, array->capacity);
    }
    array->values[array->count] = value;
    ++array->count;
}

void freeValueArray(ValueArray *array) {
    FREE_ARRAY(Value, array->values, array->capacity);
    initValueArray(array);
}

void printValue(Value value) {
    switch (value.type) {
    case VAL_BOOL:
        printf(AS_BOOL(value) ? "true" : "false");
        break;
    case VAL_NIL:
        printf("nil");
        break;
    case VAL_NUMBER:
        printf("%g", AS_NUMBER(value));
        break;
    case VAL_OBJ:
        printObject(value);
        break;
    }
}

bool valuesEqual(Value a, Value b) {
    if (a.type != b.type) { return false; }
    switch (a.type) {
    case VAL_BOOL:
        return AS_BOOL(a) == AS_BOOL(b);
    case VAL_NIL:
        return true;
    case VAL_NUMBER:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
        return AS_NUMBER(a) == AS_NUMBER(b);
#pragma GCC diagnostic pop
    case VAL_OBJ: {
        ObjString const *aString = AS_STRING(a);
        ObjString const *bString = AS_STRING(b);
        return aString->length == bString->length
               && memcmp(aString->chars, bString->chars, aString->length) == 0;
    }
    }
    __builtin_unreachable();
}
