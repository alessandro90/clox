#ifndef CLOX_VALUE_H
#define CLOX_VALUE_H

#include "common.h"
#include <string.h>

typedef struct Obj Obj;
typedef struct ObjString ObjString;

#ifdef NAN_BOXING

// NOLINTNEXTLINE
#define QNAN ((u64)0x7FFC000000000000)

// NOLINTNEXTLINE
#define SIGN_BIT ((u64)0x8000000000000000)

// NOLINTNEXTLINE
#define TAG_NIL 1  // 01.
// NOLINTNEXTLINE
#define TAG_FALSE 2  // 10.
// NOLINTNEXTLINE
#define TAG_TRUE 3  // 11.

typedef u64 Value;

// NOLINTNEXTLINE
#define IS_BOOL(value) (((value) | 1) == TRUE_VAL)

// NOLINTNEXTLINE
#define IS_NIL(value) ((value) == NIL_VAL)

// NOLINTNEXTLINE
#define IS_NUMBER(value) (((value)&QNAN) != QNAN)

// NOLINTNEXTLINE
#define IS_OBJ(value) \
    (((value) & (QNAN | SIGN_BIT)) == (QNAN | SIGN_BIT))


// NOLINTNEXTLINE
#define AS_BOOL(value) ((value) == TRUE_VAL)

// NOLINTNEXTLINE
#define AS_NUMBER(value) valueToNum(value)

// NOLINTNEXTLINE
#define AS_OBJ(value) \
    ((Obj *)(uintptr_t)((value) & ~(SIGN_BIT | QNAN)))


// NOLINTNEXTLINE
#define BOOL_VAL(b) ((b) ? TRUE_VAL : FALSE_VAL)

// NOLINTNEXTLINE
#define NIL_VAL ((Value)(u64)(QNAN | TAG_NIL))
// NOLINTNEXTLINE
#define FALSE_VAL ((Value)(u64)(QNAN | TAG_FALSE))
// NOLINTNEXTLINE
#define TRUE_VAL ((Value)(u64)(QNAN | TAG_TRUE))

// NOLINTNEXTLINE
#define NUMBER_VAL(num) numToValue(num)

// NOLINTNEXTLINE
#define OBJ_VAL(obj) \
    (Value)(SIGN_BIT | QNAN | (u64)(uintptr_t)(obj))

static inline double valueToNum(Value value) {
    double num;  // NOLINT
    memcpy(&num, &value, sizeof(Value));
    return num;
}

static inline Value numToValue(double num) {
    Value value;  // NOLINT
    memcpy(&value, &num, sizeof(double));
    return value;
}

#else

typedef enum {
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER,
    VAL_OBJ,
} ValueType;

typedef struct {
    ValueType type;
    union {
        bool boolean;
        double number;
        Obj *obj;
    } as;
} Value;


// NOLINTNEXTLINE
#define IS_BOOL(value) ((value).type == VAL_BOOL)
// NOLINTNEXTLINE
#define IS_NIL(value) ((value).type == VAL_NIL)
// NOLINTNEXTLINE
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)
// NOLINTNEXTLINE
#define IS_OBJ(value) ((value).type == VAL_OBJ)

// NOLINTNEXTLINE
#define AS_BOOL(value) ((value).as.boolean)
// NOLINTNEXTLINE
#define AS_NUMBER(value) ((value).as.number)
// NOLINTNEXTLINE
#define AS_OBJ(value) ((value).as.obj)


// NOLINTNEXTLINE
#define BOOL_VAL(value) ((Value){VAL_BOOL, {.boolean = (value)}})
// NOLINTNEXTLINE
#define NIL_VAL ((Value){VAL_NIL, {.number = 0}})
// NOLINTNEXTLINE
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = (value)}})
// NOLINTNEXTLINE
#define OBJ_VAL(value) ((Value){VAL_OBJ, {.obj = (Obj *)(value)}})

#endif

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
