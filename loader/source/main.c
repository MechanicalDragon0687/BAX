#include "fatfs/ff.h"

#include <string.h>
#include <stdlib.h>

const char payload_name[] = "anim/arm9payload.bin", luma_payload[] = "anim/luma.bin";
const unsigned int dest_addr = 0x23F00000; // Address to load the payloads at

struct framebuffer_t { // Thanks to mid-kid for the framebuffer structure
    unsigned char *top_left;
    unsigned char *top_right;
    unsigned char *bottom;
};
struct framebuffer_t* framebuffers = (struct framebuffer_t *) 0x23FFFE00;

void white_screen() { // Display a white top screen    
	memset(framebuffers->top_left, 0xFF, 0x46500);
	while (1); // u dun goof, inf loop
}

static unsigned char *memsearch(unsigned char *startPos, const void *pattern, int size, int patternSize) // From Luma3DS' pathchanger.c (literally copy-pasted)
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
	char buf[74] = {0}; // [DOH's in german]

	unsigned char *found = memsearch(start, orig, len, sizeof(orig)); // Search for UTF-16 pattern ("sdmc:/")

	if (found == NULL)
	{
		return; // Welp, not found. Nothing to see here folks...
	}

	for (unsigned int i = 0; i < sizeof(luma_payload) - 1; i++) // Convert from ASCII/UTF-8 to UTF-16
	{
		buf[2 * i] = luma_payload[i];
	}

	memcpy(found + 12, buf, sizeof(buf)); // Copy the patched path over

	return;
}

void main() {
    FATFS fs;
    FIL payload_file;
    unsigned int br = 0;

    if (f_mount(&fs, "0:", 1) != FR_OK) // Mount the SD
	{
		white_screen(); // If not successful, white screen
	}

	if(f_open(&payload_file, payload_name, FA_READ) == FR_OK) // Attempt to open "arm9payload.bin"
	{
		f_read(&payload_file, (void*)dest_addr, f_size(&payload_file), &br); // If opened, read to the destination address
		f_close(&payload_file);
		((void (*)(void))dest_addr)(); // Jump there
    }

	else if (f_open(&payload_file, luma_payload, FA_READ) == FR_OK) // Attempt to open "luma.bin"
	{
		f_read(&payload_file, (void*)dest_addr, f_size(&payload_file), &br); // Read...
		f_close(&payload_file);

		if (br <= 0x20000) // 200kb is the max Luma3DS payload size defined in reboot.s
		// Until it's updated with a larger filesize, this will do
		{
			patch_luma((void*)dest_addr, br); // Patch
		}

		((void (*)(void))dest_addr)(); // Jump
	}

	white_screen(); // well, tough luck mate. Basically, no payload found.
}
