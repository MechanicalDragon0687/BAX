#include "memory.h"

void memcpy(void *dest, void *src, u32 size) {
	u8* dst8 = (u8*)dest;
	u8* src8 = (u8*)src;

	while (size--)
		*dst8++ = *src8++;

	return;
}

void memset(void *dest, u32 filler, u32 size) {
    u8 *destc = (u8 *)dest;

    while(size--)
		*destc++ = filler;

	return;
}
