# Clox

Implementation of the Lox programming language as described in the book [Crafting Interpreters](https://www.craftinginterpreters.com) by Robert Nystrom.
The code is almost identical to that written in the book apart for a couple of type aliases and the ordering of some function definitions/implementations, switch/case statements etc...

## Build (Cmake)

**NOTE** The code should be standard C and compile with every compiler, yet I compiled and run it only on gcc (13) on Linux.

Create a build directory and `cd` into it:

```bash
mkdir build
cd build
```

Create a Makefile (choose between release and debug mode, release in the example):

```bash
cmake -DCMAKE_BUILD_TYPE=Relese ..
```

Compile the program:

```bash
make
```
