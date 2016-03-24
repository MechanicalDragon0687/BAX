#pragma once

#include "types.h"

u32 mountSDMC(void);
u32 fileReadOffset(u8 *dest, const char *path, u32 offset, u32 size);
u32 fileRead(u8 *dest, const char *path, u32 size);
u32 fileSize(const char *path);
u32 fileExists(const char *path);
