#pragma once

#include <common.h>

void console_reset(u16 *base, int width);
void console_putc(char c);
void console_puts(const char *s);

void console_puth(u32 h);
void console_putd(u32 d);
