#ifndef CLOX_DEBUG_H
#define CLOX_DEBUG_H

#include "chunk.h"
#include <stdint.h>

void disassembleChunk(Chunk const *chunk, const char *name);
size_t disassembleInstruction(Chunk const *chunk, size_t offset);

#endif
