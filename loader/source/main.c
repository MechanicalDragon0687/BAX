#include "fatfs/ff.h"

#include <string.h>
#include <stdlib.h>

const char payload_name[] = "anim/arm9payload.bin";
const unsigned int dest_addr = 0x23F00000; // Address to load the payloads at

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

	unsigned char *found = memsearch(start, orig, len, sizeof(orig)); // Search for UTF-16 pattern ("sdmc:/")

	if (found == NULL)
		return; // Welp, not found. Nothing to see here folks...

	for (unsigned int i = 0; i < sizeof(payload_name) - 1; i++) // Convert from ASCII/UTF-8 to UTF-16
	{
		//buf[2 * i] = payload_name[i];
        found[12 + (2 * i)] = payload_name[i];
	}

	return;
}

void main() {
    FATFS fs;
    FIL payload_file;
    unsigned int br = 0;

    if (f_mount(&fs, "0:", 1) != FR_OK) // Mount the SD
	{
		while(1);
	}

	if(f_open(&payload_file, payload_name, FA_READ) == FR_OK) // Attempt to open "arm9payload.bin"
	{
		f_read(&payload_file, (void*)dest_addr, f_size(&payload_file), &br); // If opened, read to the destination address
		f_close(&payload_file);

        if (br <= 0x20000) // 200kb is the max Luma3DS payload size defined in reboot.s
		// Until it's updated with a larger filesize, this will do
		{
			patch_luma((void*)dest_addr, br); // Patch
		}

		((void (*)(void))dest_addr)();
    }

	while(1);
}
