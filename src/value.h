#ifndef CLOX_VALUE_H
#define CLOX_VALUE_H

#include "common.h"

typedef enum {
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER,
} ValueType;

typedef struct {
    ValueType type;
    union {
        bool boolean;
        double number;
    } as;
} Value;

// NOLINTNEXTLINE
#define IS_BOOL(value) ((value).type == VAL_BOOL)
// NOLINTNEXTLINE
#define IS_NIL(value) ((value).type == VAL_NIL)
// NOLINTNEXTLINE
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)

// NOLINTNEXTLINE
#define AS_BOOL(value) ((value).as.boolean)
// NOLINTNEXTLINE
#define AS_NUMBER(value) ((value).as.number)


// NOLINTNEXTLINE
#define BOOL_VAL(value) ((Value){VAL_BOOL, {.boolean = (value)}})
// NOLINTNEXTLINE
#define NIL_VAL ((Value){VAL_NIL, {.number = 0}})
// NOLINTNEXTLINE
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = (value)}})

typedef struct {
    usize capacity;
    usize count;
    Value *values;
} ValueArray;

bool valuesEqual(Value a, Value b);
void initValueArray(ValueArray *array);
void writeValueArray(ValueArray *array, Value value);
void freeValueArray(ValueArray *array);

void printValue(Value value);

#endif
