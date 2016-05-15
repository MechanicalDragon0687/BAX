#include "fatfs/ff.h"

#include <string.h>
#include <stdlib.h>

const char payload_name[] = "anim/arm9payload.bin", luma_payload[] = "anim/luma.bin";
const unsigned int dest_addr = 0x23F00000;

struct framebuffer_t { // Thanks to mid-kid for the framebuffer structure
    unsigned char *top_left;
    unsigned char *top_right;
    unsigned char *bottom;
};
struct framebuffer_t* framebuffers = (struct framebuffer_t *) 0x23FFFE00;

void white_screen() { // Display a white screen    
	memset(framebuffers->top_left, 0xFF, 288000);
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
	char buf[2 * (sizeof(luma_payload) - 1)] = {0};

	unsigned char *found = memsearch(start, orig, len, sizeof(orig));

	if (found == NULL)
	{
		return;
	}

	for (unsigned int i = 0; i < sizeof(luma_payload) - 1; i++)
	{
		buf[2 * i] = luma_payload[i];
	}

	memcpy(found + 12, buf, sizeof(buf));
	memset(found + 12 + sizeof(buf), 0, 74 - sizeof(buf)); // Zero out the rest of the payload name

	return;
}

void main() {
    FATFS fs;
    FIL payload_file;
    unsigned int br = 0;

    if (f_mount(&fs, "0:", 1) != FR_OK)
	{
		white_screen();
	}

	if(f_open(&payload_file, payload_name, FA_READ) == FR_OK)
	{
		f_read(&payload_file, (void*)dest_addr, f_size(&payload_file), &br);
		f_close(&payload_file);
		((void (*)(void))dest_addr)();
    }

	else if (f_open(&payload_file, luma_payload, FA_READ) == FR_OK)
	{
		f_read(&payload_file, (void*)dest_addr, f_size(&payload_file), &br);
		f_close(&payload_file);
		if (br < 0x20000) // Pathchanger does it, so should I
		{
			patch_luma((void*)dest_addr, br);
		}

		((void (*)(void))dest_addr)();
	}

	white_screen();
}
