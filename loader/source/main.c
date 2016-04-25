#include "fatfs/ff.h"

#define PAYLOAD_ADDRESS	0x23F00000

const char* payload = "/anim/arm9payload.bin";

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
}

void main() {
    FATFS fs;
    FIL payload_file;
    unsigned int br = 0;

    if (f_mount(&fs, "0:", 1) != FR_OK)
		white_screen();

	if(f_open(&payload_file, payload, FA_READ) == FR_OK) {
		f_read(&payload_file, (void *)PAYLOAD_ADDRESS, f_size(&payload_file), &br);
		((void (*)(void))PAYLOAD_ADDRESS)();
    }

	else
		white_screen();
}
