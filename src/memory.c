#include "memory.h"
#include "common.h"
#include <stdlib.h>

void *reallocate(void *pointer, usize oldSize, usize newSize) {
    (void)oldSize;
    if (newSize == 0) {
        free(pointer);
        return NULL;
    }
    void *result = realloc(pointer, newSize);
    if (result == NULL) {
        exit(1);  // NOLINT
    }
    return result;
}
