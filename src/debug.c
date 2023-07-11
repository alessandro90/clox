#include "debug.h"
#include "chunk.h"
#include "common.h"
#include "object.h"
#include "value.h"
#include <stdio.h>

static usize simpleInstruction(const char *name, usize offset) {
    printf("%s\n", name);
    return offset + 1U;
}

static usize byteInstruction(const char *name, Chunk const *chunk, usize offset) {
    u8 const slot = chunk->code[offset + 1U];
    printf("%-16s %4u\n", name, slot);
    return offset + 2U;
}

static usize jumpInstruction(const char *name, i32 sign, Chunk const *chunk, usize offset) {
    u16 jump = (u16)(chunk->code[offset + 1U] << 8U);  // NOLINT
    jump |= chunk->code[offset + 2U];
    printf("%-16s %4lu -> %d\n", name, offset, (i32)offset + 3 + sign * (i32)jump);
    return offset + 3U;
}

static usize constantInstruction(const char *name, Chunk const *chunk, usize offset) {
    u8 const constant = chunk->code[offset + 1U];
    printf("%-16s %4d '", name, constant);
    printValue(chunk->constants.values[constant]);
    printf("'\n");
    return offset + 2U;
}

void disassembleChunk(Chunk const *chunk, const char *name) {
    printf("== %s ==\n", name);
    for (usize offset = 0; offset < chunk->count;) {
        offset = disassembleInstruction(chunk, offset);
    }
}

usize disassembleInstruction(Chunk const *chunk, usize offset) {
    printf("%04lu ", offset);
    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1U]) {
        printf("   | ");
    } else {
        printf("%4lu ", chunk->lines[offset]);
    }
    u8 const instruction = chunk->code[offset];
    switch (instruction) {
    case OP_CONSTANT:
        return constantInstruction("OP_CONSTANT", chunk, offset);
    case OP_NIL:
        return simpleInstruction("OP_NIL", offset);
    case OP_TRUE:
        return simpleInstruction("OP_TRUE", offset);
    case OP_FALSE:
        return simpleInstruction("OP_FALSE", offset);
    case OP_ADD:
        return simpleInstruction("OP_ADD", offset);
    case OP_SUBTRACT:
        return simpleInstruction("OP_SUBTRACT", offset);
    case OP_MULTIPLY:
        return simpleInstruction("OP_MULTIPLY", offset);
    case OP_DIVIDE:
        return simpleInstruction("OP_DIVIDE", offset);
    case OP_NEGATE:
        return simpleInstruction("OP_NEGATE", offset);
    case OP_PRINT:
        return simpleInstruction("OP_PRINT", offset);
    case OP_RETURN:
        return simpleInstruction("OP_RETURN", offset);
    case OP_NOT:
        return simpleInstruction("OP_NOT", offset);
    case OP_EQUAL:
        return simpleInstruction("OP_EQUAL", offset);
    case OP_GREATER:
        return simpleInstruction("OP_GREATER", offset);
    case OP_LESS:
        return simpleInstruction("OP_LESS", offset);
    case OP_POP:
        return simpleInstruction("OP_POP", offset);
    case OP_DEFINE_GLOBAL:
        return constantInstruction("OP_DEFINE_GLOBAL", chunk, offset);
    case OP_GET_GLOBAL:
        return constantInstruction("OP_GET_GLOBAL", chunk, offset);
    case OP_SET_GLOBAL:
        return constantInstruction("OP_SET_GLOBAL", chunk, offset);
    case OP_GET_LOCAL:
        return byteInstruction("OP_GET_LOCAL", chunk, offset);
    case OP_SET_LOCAL:
        return byteInstruction("OP_SET_LOCAL", chunk, offset);
    case OP_GET_UPVALUE:
        return byteInstruction("OP_GET_VALUE", chunk, offset);
    case OP_SET_UPVALUE:
        return byteInstruction("OP_SET_VALUE", chunk, offset);
    case OP_JUMP_IF_FALSE:
        return jumpInstruction("OP_JUMP_IF_FALSE", 1, chunk, offset);
    case OP_JUMP:
        return jumpInstruction("OP_JUMP", 1, chunk, offset);
    case OP_LOOP:
        return jumpInstruction("OP_LOOP", -1, chunk, offset);
    case OP_CALL:
        return byteInstruction("OP_CALL", chunk, offset);
    case OP_CLOSURE: {
        ++offset;
        u8 const constant = chunk->code[offset++];
        printf("%-16s %4d ", "OP_CLOSURE", constant);
        printValue(chunk->constants.values[constant]);
        printf("\n");
        ObjFunction *function = AS_FUNCTION(chunk->constants.values[constant]);
        for (usize j = 0; j < function->upvalueCount; ++j) {
            i32 const isLocal = chunk->code[offset++];
            i32 const index = chunk->code[offset++];
            printf("%04lu      |                     %s %d\n", offset - 2U, isLocal ? "local" : "upvalue", index);
        }
        return offset;
    }
    }
    printf("Unknown opcode %d\n", (i32)instruction);
    return offset + 1U;
}
