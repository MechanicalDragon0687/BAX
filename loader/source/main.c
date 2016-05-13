#include "fatfs/ff.h"

#include <string.h>
#include <stdlib.h>

#define PAYLOAD_ADDRESS	0x23F00000

#define payload_name   "/anim/arm9payload.bin"
#define luma_flag_name "/anim/luma"

struct framebuffer_t { // Thanks to mid-kid for the framebuffer structure
    unsigned char *top_left;
    unsigned char *top_right;
    unsigned char *bottom;
};
struct framebuffer_t* framebuffers = (struct framebuffer_t *) 0x23FFFE00;

void white_screen() { // Display a white screen
	int size = (400*240*3); // Top framebuffer size
    while(size--)
		*(framebuffers->top_left + size) = 0xFF; // 0xFF = 255 = White
	
	while (1);
}

static unsigned char *memsearch(unsigned char *startPos, const void *pattern, int size, int patternSize) // From Luma3DS' pathchanger.c
{
    const unsigned char *patternc = (const unsigned char *)pattern;

    //Preprocessing
    int table[256];

    int i;
    for(i = 0; i < 256; ++i)
        table[i] = patternSize + 1;
    for(i = 0; i < patternSize; ++i)
        table[patternc[i]] = patternSize - i;

    //Searching
    int j = 0;

    while(j <= size - patternSize)
    {
        if(memcmp(patternc, startPos + j, patternSize) == 0)
            return startPos + j;
        j += table[startPos[j + patternSize]];
    }

    return NULL;
}

void patch_luma(void *start, size_t len)
{
	char orig[] = {'s', 0, 'd', 0, 'm', 0, 'c', 0, ':', 0, '/', 0};
	char patched[] = "anim/arm9payload.bin";
	char buf[2 * (sizeof(patched) - 1)] = {0};

	unsigned char *found = memsearch(start, orig, len, sizeof(orig));

	if (found == NULL)
	{
		return;
	}

	for (unsigned int i = 0; i < sizeof(patched) - 1; i++)
	{
		buf[2 * i] = patched[i];
	}

	memcpy(found + 12, buf, sizeof(buf));

	return;
}

void main() {
    FATFS fs;
    FIL payload_file, luma;
    unsigned int br = 0;

    if (f_mount(&fs, "0:", 1) != FR_OK)
	{
		white_screen();
	}

	if(f_open(&payload_file, payload_name, FA_READ) == FR_OK)
	{
		f_read(&payload_file, (void*)PAYLOAD_ADDRESS, f_size(&payload_file), &br);

		if (f_open(&luma, luma_flag_name, FA_READ) != FR_OK && br < 0x20000)
		{
			f_close(&luma);
			patch_luma((void*)PAYLOAD_ADDRESS, br);  // Only useful for Luma3DS, hopefully no other payload requires such a hack
		}

		((void (*)(void))PAYLOAD_ADDRESS)();
    }

	else
	{
		white_screen();
	}
}
