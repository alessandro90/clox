#include "vm.h"
#include "common.h"
#include "debug.h"
#include "stdio.h"
#include "value.h"

VM vm;  // NOLINT

static void resetStack(void) {
    vm.stackTop = vm.stack;
}

static InterpretResult run(void) {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op)    \
    do {                 \
        Value b = pop(); \
        Value a = pop(); \
        push(a op b);    \
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
        case OP_ADD:
            BINARY_OP(+);
            break;
        case OP_SUBTRACT:
            BINARY_OP(-);
            break;
        case OP_MULTIPLY:
            BINARY_OP(*);
            break;
        case OP_DIVIDE:
            BINARY_OP(/);
            break;
        case OP_NEGATE:
            push(-pop());
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

InterpretResult interpret(Chunk *chunk) {
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run();
}
