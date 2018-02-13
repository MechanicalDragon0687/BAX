/**
Copyright 2018 Wolfvak

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*/

#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <stdarg.h>

#define MAKEBAX_VERSION "0.1a"

#define FRAME_H        (240)
#define FRAME_SIZE_H   (FRAME_H * 2)
#define FRAME_SIZE_MAX (FRAME_SIZE_H * 720)


#define power_of_two(x) (!((x) & ((x) - 1)))
static inline size_t Align_Up(size_t n, size_t a)
{
    if (power_of_two(a)) {
        n += (a - 1);
        n &= ~(a - 1);
    } else if ((n % a) != 0) {
        n = (((n + a) / a) * a);
    }
    return n;
}

static inline size_t Align_Down(size_t n, size_t a)
{
    if (power_of_two(a)) {
        n &= ~(a - 1);
    } else {
        n -= (n % a);
    }
    return n;
}

static void Abort_Error(const char *str, ...)
{
    va_list va;
    va_start(va, str);
    vprintf(str, va);
    va_end(va);

    std::cout << "Aborted\n";

    exit(EXIT_FAILURE);
}

#endif
