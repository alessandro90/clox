#ifndef CLOX_VM_H
#define CLOX_VM_H

#include "chunk.h"
#include "common.h"
#include "table.h"
#include "value.h"

#define STACK_MAX 256

typedef struct {
    Chunk *chunk;
    u8 *ip;
    Value stack[STACK_MAX];
    Value *stackTop;
    Table strings;
    Obj *objects;
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