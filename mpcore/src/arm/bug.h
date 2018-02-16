#ifndef BUG_H
#define BUG_H

#include <types.h>

#define BUGSTR(...) ((const char*[]){__VA_ARGS__})
#define BUGINT(...) ((u32[]){__VA_ARGS__})

void __attribute__((noreturn)) BUG(const char **strs, int sc, u32 *n, int nc);

#endif // BUG_H
