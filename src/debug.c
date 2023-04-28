#include "debug.h"
#include "value.h"
#include <stdint.h>
#include <stdio.h>

static size_t simpleInstruction(const char *name, size_t offset) {
    printf("%s\n", name);
    return offset + 1U;
}

static size_t constantInstruction(const char *name, Chunk const *chunk, size_t offset) {
    uint8_t const constant = chunk->code[offset + 1U];
    printf("%-16s %4d '", name, constant);
    printValue(chunk->constants.values[constant]);
    printf("'\n");
    return offset + 2U;
}

void disassembleChunk(Chunk const *chunk, const char *name) {
    printf("== %s ==\n", name);
    for (size_t offset = 0; offset < chunk->count;) {
        offset = disassembleInstruction(chunk, offset);
    }
}

size_t disassembleInstruction(Chunk const *chunk, size_t offset) {
    printf("%04lu ", offset);
    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1U]) {
        printf("   | ");
    } else {
        printf("%4lu ", chunk->lines[offset]);
    }
    uint8_t const instruction = chunk->code[offset];
    switch (instruction) {
    case OP_RETURN:
        return simpleInstruction("OP_RETURN", offset);
    case OP_CONSTANT:
        return constantInstruction("OP_CONSTANT", chunk, offset);
    }
    printf("Unknown opcode %d\n", (int)instruction);
    return offset + 1U;
}
