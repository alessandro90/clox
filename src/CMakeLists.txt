cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wdouble-promotion
    -Wformat=2
    -Wformat-security
    -Wformat-signedness
    -Wnull-dereference
    -Wtrivial-auto-var-init
    -Wunused-parameter
    -Wunused-const-variable=2
    -Wuninitialized
    -Wmaybe-uninitialized
    -Wstringop-overflow=4
    -Wconversion
    -Wfloat-conversion
    -Wsign-conversion
    -Warith-conversion
    -Wbool-compare
    -Wduplicated-branches
    -Wduplicated-cond
    -Wfloat-equal
    -Wshadow
    -Wundef
    -Wunused-macros
    -Wcast-qual
    -Wcast-align=strict
    -Wlogical-op
    -Wmissing-declarations
    -Wredundant-decls
    # -Winline
    -Wlong-long
    -Wimplicit-fallthrough=5
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -g")

set(
    SRC_FILES
    main.c
    chunk.c
    memory.c
    debug.c
    value.c
    vm.c
)

add_executable(clox ${SRC_FILES})
