#ifndef CLOX_CHUNK_H
#define CLOX_CHUNK_H

#include "common.h"
#include "value.h"

typedef enum {
    OP_RETURN,
    OP_CONSTANT,
} OpCode;

typedef struct {
    size_t count;
    size_t capacity;
    uint8_t *code;
    size_t *lines;
    ValueArray constants;
} Chunk;

void initChunk(Chunk *chunk);
void freeChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, uint8_t byte, size_t line);
uint8_t addConstant(Chunk *chunk, Value value);

#endif
