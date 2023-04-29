#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "vm.h"

int main(int argc, const char **args) {
    (void)argc;
    (void)args;

    initVM();

    Chunk chunk;
    initChunk(&chunk);

    u8 constant = addConstant(&chunk, 1.2);  // NOLINT
    writeChunk(&chunk, OP_CONSTANT, 123);  // NOLINT
    writeChunk(&chunk, constant, 123);  // NOLINT

    constant = addConstant(&chunk, 3.4);  // NOLINT
    writeChunk(&chunk, OP_CONSTANT, 123);  // NOLINT
    writeChunk(&chunk, constant, 123);  // NOLINT

    writeChunk(&chunk, OP_ADD, 123);  // NOLINT

    constant = addConstant(&chunk, 5.6);  // NOLINT
    writeChunk(&chunk, OP_CONSTANT, 123);  // NOLINT
    writeChunk(&chunk, constant, 123);  // NOLINT

    writeChunk(&chunk, OP_DIVIDE, 123);  // NOLINT


    writeChunk(&chunk, OP_NEGATE, 123);  // NOLINT

    writeChunk(&chunk, OP_RETURN, 123);  // NOLINT

    interpret(&chunk);

    freeVM();
    freeChunk(&chunk);
    return 0;
}
