#ifndef CLOX_MEMORY_H
#define CLOX_MEMORY_H

#include "common.h"

// NOLINTNEXTLINE
#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity)*2)


// NOLINTNEXTLINE
#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    (type *)reallocate(pointer, sizeof(type) * (oldCount), sizeof(type) * (newCount))


// NOLINTNEXTLINE
#define FREE_ARRAY(type, pointer, oldCount) \
    reallocate(pointer, sizeof(type) * (oldCount), 0)

void *reallocate(void *pointer, size_t oldSize, size_t newSize);

#endif
