#ifndef CLOX_COMMON_H
#define CLOX_COMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef uint8_t u8;
typedef int8_t i8;

typedef uint16_t u16;
typedef int16_t i16;

typedef uint32_t u32;
typedef int32_t i32;

typedef uint64_t u64;
typedef int64_t i64;

typedef size_t usize;

// NOLINTNEXTLINE
#define DEBUG_TRACE_EXECUTION

// NOLINTNEXTLINE
// #define DEBUG_STRESS_GC

// NOLINTNEXTLINE
// #define DEBUG_LOG_GC

// NOLINTNEXTLINE
#define DEBUG_PRINT_CODE

// NOLINTNEXTLINE
#define UINT8_COUNT (UINT8_MAX + 1U)

#endif
