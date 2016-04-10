#include "fatfs/ff.h"
#include "draw.h"
#include <stddef.h>

#define PAYLOAD_ADDRESS	0x23F00000

const char* payload = "/anim/arm9payload.bin";

struct framebuffer_t* framebuffers = (struct framebuffer_t *) 0x23FFFE00;

void whiteScreen() { // Display a white screen
	unsigned char *destc = framebuffers->top_left;
	int size = (400*240*3); // Top framebuffer size
    while(size--)
		*destc++ = 0xFF; // 0xFF = 255 = White
}

int fileExists(const char * f_path) { // Check whether file exists
	FIL test;
	if (f_open(&test, f_path, FA_READ) == FR_OK) {
		f_close(&test);
		return 1;
	}

	else return 0;
}

void main() {
    FATFS fs;
    FIL payload_file;
    unsigned int br;

    f_mount(&fs, "0:", 1);

	if (!fileExists(payload))
		whiteScreen();

	if(f_open(&payload_file, payload, FA_READ) == FR_OK) {
		f_read(&payload_file, (void *)PAYLOAD_ADDRESS, f_size(&payload_file), &br);
		((void (*)())PAYLOAD_ADDRESS)();
    }

	else whiteScreen();
}
