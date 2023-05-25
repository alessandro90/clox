#ifndef CLOX_CHUNK_H
#define CLOX_CHUNK_H

#include "common.h"
#include "value.h"

typedef enum {
    OP_CONSTANT,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NOT,
    OP_NEGATE,
    OP_PRINT,
    OP_RETURN,
    OP_POP,
    OP_DEFINE_GLOBAL,
    OP_GET_GLOBAL,
    OP_SET_GLOBAL,
    OP_GET_LOCAL,
    OP_SET_LOCAL,
    OP_JUMP_IF_FALSE,
} OpCode;

typedef struct {
    usize count;
    usize capacity;
    u8 *code;
    usize *lines;
    ValueArray constants;
} Chunk;

void initChunk(Chunk *chunk);
void freeChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, u8 byte, usize line);
usize addConstant(Chunk *chunk, Value value);

#endif
