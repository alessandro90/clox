#include "vm.h"
#include "chunk.h"
#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "stdarg.h"
#include "stdio.h"
#include "value.h"

VM vm;  // NOLINT

static void resetStack(void) {
    vm.stackTop = vm.stack;
}

static void runtimeError(const char *format, ...) {
    va_list args;
    va_start(args, format);
    (void)vfprintf(stderr, format, args);  // NOLINT
    va_end(args);
    (void)fputs("\n", stderr);
    usize const instruction = (usize)(vm.ip - vm.chunk->code - 1U);
    usize const line = vm.chunk->lines[instruction];
    (void)fprintf(stderr, "[line %d] in script\n", (i32)line);
    resetStack();
}

static Value peek(i32 distance) {
    return vm.stackTop[-1 - distance];
}

static bool isFalsey(Value value) {
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
static InterpretResult run(void) {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(valueType, op)                          \
    do {                                                  \
        if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
            runtimeError("Operands must be numbers.");    \
            return INTERPRET_COMPILE_ERROR;               \
        }                                                 \
        double const b = AS_NUMBER(pop());                \
        double const a = AS_NUMBER(pop());                \
        push(valueType(a op b));                          \
    } while (false)

    while (true) {
#ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        for (Value *slot = vm.stack; slot < vm.stackTop; ++slot) {
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
        printf("\n");
        disassembleInstruction(vm.chunk, (usize)(vm.ip - vm.chunk->code));
#endif
        u8 const instruction = READ_BYTE();
        switch (instruction) {
        case OP_CONSTANT: {
            Value const constant = READ_CONSTANT();
            push(constant);
            break;
        }
        case OP_NIL:
            push(NIL_VAL);
            break;
        case OP_TRUE:
            push(BOOL_VAL(true));
            break;
        case OP_FALSE:
            push(BOOL_VAL(false));
            break;
        case OP_EQUAL: {
            Value const b = pop();
            Value const a = pop();
            push(BOOL_VAL(valuesEqual(a, b)));
            break;
        }
        case OP_GREATER:
            BINARY_OP(BOOL_VAL, >);
            break;
        case OP_LESS:
            BINARY_OP(BOOL_VAL, <);
            break;
        case OP_ADD:
            BINARY_OP(NUMBER_VAL, +);
            break;
        case OP_SUBTRACT:
            BINARY_OP(NUMBER_VAL, -);
            break;
        case OP_MULTIPLY:
            BINARY_OP(NUMBER_VAL, *);
            break;
        case OP_DIVIDE:
            BINARY_OP(NUMBER_VAL, /);
            break;
        case OP_NOT:
            push(BOOL_VAL(isFalsey(pop())));
            break;
        case OP_NEGATE:
            if (!IS_NUMBER(peek(0))) {
                runtimeError("Operand must be a number.");
                return INTERPRET_RUNTIME_ERROR;
            }
            push(NUMBER_VAL(-AS_NUMBER(pop())));
            break;
        case OP_RETURN:
            printValue(pop());
            printf("\n");
            return INTERPRET_OK;
        }
    }

#undef BINARY_OP
#undef READ_CONSTANT
#undef READ_BYTE
}

void initVM(void) {
    resetStack();
}

void freeVM(void) {
}

void push(Value value) {
    *vm.stackTop = value;
    ++vm.stackTop;
}

Value pop(void) {
    --vm.stackTop;
    return *vm.stackTop;
}

InterpretResult interpret(const char *source) {
    Chunk chunk;
    initChunk(&chunk);

    if (!compile(source, &chunk)) {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult const result = run();

    freeChunk(&chunk);
    return result;
}
