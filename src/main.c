#include "chunk.h"
#include "common.h"
#include "debug.h"

int main(int argc, const char **args) {
    (void)argc;
    (void)args;
    Chunk chunk;
    initChunk(&chunk);

    uint8_t constant = addConstant(&chunk, 1.2);  // NOLINT
    writeChunk(&chunk, OP_CONSTANT, 123);  // NOLINT
    writeChunk(&chunk, constant, 123);  // NOLINT

    writeChunk(&chunk, OP_RETURN, 123);  // NOLINT

    disassembleChunk(&chunk, "test chunk");

    freeChunk(&chunk);
    return 0;
}
