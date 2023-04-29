#ifndef CLOX_DEBUG_H
#define CLOX_DEBUG_H

#include "chunk.h"
#include "common.h"

void disassembleChunk(Chunk const *chunk, const char *name);
usize disassembleInstruction(Chunk const *chunk, usize offset);

#endif
