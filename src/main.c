#include "common.h"
#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *readFile(const char *path) {
    FILE *file = fopen(path, "rbe");
    if (file == NULL) {
        fprintf(stderr, "Cannot open file %s\n", path);  // NOLINT
        exit(EXIT_FAILURE);  // NOLINT
    }
    i32 const found = fseek(file, 0, SEEK_END);
    if (found != 0) {
        fprintf(stderr, "seek function failed for %s with %d\n", path, found);  // NOLINT
        exit(EXIT_FAILURE);  // NOLINT
    }
    i64 const fileSize = ftell(file);
    if (fileSize == -1L) {
        fprintf(stderr, "ftell function failed for %s with %ld\n", path, fileSize);  // NOLINT
        exit(EXIT_FAILURE);  // NOLINT
    }
    rewind(file);

    char *buffer = (char *)malloc((u64)fileSize + 1UL);
    if (buffer == NULL) {
        fprintf(stderr, "malloc function failed for %s\n", path);  // NOLINT
        exit(EXIT_FAILURE);  // NOLINT
    }

    usize const bytesRead = fread(buffer, sizeof(char), (u64)fileSize, file);
    if (bytesRead != (usize)fileSize) {
        fprintf(stderr, "fread function failed for %s\n", path);  // NOLINT
        exit(EXIT_FAILURE);  // NOLINT
    }

    buffer[bytesRead] = '\0';
    i32 const closed = fclose(file);
    if (closed != 0) {
        fprintf(stderr, "cannot close %s file. Continuing execution\n", path);  // NOLINT
    }
    return buffer;
}

static void repl(void) {
    const usize maxLines = 1024;
    char line[maxLines];
    while (true) {
        printf("> ");
        if (!fgets(line, (i32)sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        interpret(line);
    }
}

static void runFile(const char *path) {
    char *source = readFile(path);
    InterpretResult const result = interpret(source);
    free(source);

    if (result == INTERPRET_COMPILE_ERROR) exit(65);  // NOLINT
    if (result == INTERPRET_INTERPRET_RUNTIME_ERROR) exit(70);  // NOLINT
}

int main(int argc, const char **argv) {
    initVM();

    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        runFile(argv[1]);
    } else {
        i32 const hasError = fprintf(stderr, "Usage: clox [path]\n");
        if (hasError < 0) {
            printf("Internal error in fprintf: %d\n", hasError);
        }
        exit(64);  // NOLINT
    }

    freeVM();
    return 0;
}
