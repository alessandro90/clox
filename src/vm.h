#ifndef CLOX_VM_H
#define CLOX_VM_H

#include "chunk.h"
#include "common.h"
#include "object.h"
#include "table.h"
#include "value.h"

#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)

// Represents a single ongoing function call
typedef struct {
    ObjClosure *closure;
    u8 *ip;
    Value *slots;
} CallFrame;

typedef struct {
    CallFrame frames[FRAMES_MAX];
    i32 frameCount;
    Value stack[STACK_MAX];
    Value *stackTop;
    Table globals;
    Table strings;
    ObjString *initString;
    ObjUpvalue *openUpvalues;
    usize bytesAllocated;
    usize nextGC;
    Obj *objects;
    usize grayCount;
    usize grayCapacity;
    Obj **grayStack;
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
} InterpretResult;

extern VM vm;  // NOLINT

void initVM(void);
void freeVM(void);

InterpretResult interpret(const char *source);
void push(Value value);
Value pop(void);

#endif
