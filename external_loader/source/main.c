#include "fatfs/ff.h"

#define PAYLOAD_ADDRESS	0x23F00000

void main()
{
    FATFS fs;
    FIL payload;
    unsigned int br;
	
    f_mount(&fs, "0:", 1);
    if(f_open(&payload, "sdmc:/arm9payload.bin", FA_READ) == FR_OK)
    {
        f_read(&payload, (void *)PAYLOAD_ADDRESS, f_size(&payload), &br);
        ((void (*)())PAYLOAD_ADDRESS)();
    }
}
