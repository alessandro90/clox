#include "compiler.h"
#include "common.h"
#include "scanner.h"
#include <stdio.h>

void compile(const char *source) {
    initScanner(source);
    usize line = 0;
    while (true) {
        Token const token = scanToken();
        if (token.line != line) {
            printf("%4lu ", token.line);
            line = token.line;
        } else {
            printf("   | ");
        }

        printf("%2d '%.*s'\n", (i32)token.type, (i32)token.length, token.start);

        if (token.type == TOKEN_EOF) {
            break;
        }
    }
}
