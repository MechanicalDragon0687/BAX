#include "fatfs/ff.h"
#include "draw.h"
#include <stddef.h>

#define PAYLOAD_ADDRESS	0x23F00000

char *payloads[] = {"arm9payload.bin", "anim/arm9payload.bin", "aurei/arm9payload.bin", "rei/arm9payload.bin", "/NANDUnBricker.bin"};
int n_payloads = 5;

struct framebuffer_t* framebuffers = (struct framebuffer_t *) 0x23FFFE00;

void whiteScreen() { // Display a white screen
	unsigned char *destc = framebuffers->top_left;
	int size = (400*240*3); // Top framebuffer size
    while(size--)
		*destc++ = 0xFF; // 0xFF = 255 = White
}

int fileExists(char * f_path) { // Check whether file exists
	FIL test;
	if (f_open(&test, f_path, FA_READ) == FR_OK) {
		f_close(&test);
		return 1;
	}

	else return 0;
}

char * checkPayloads() { // Check whether the payload exists, returns the name if so, otherwise returns NULL
	for (int i = 0; i <= (n_payloads - 1); i++) {
		if (fileExists(payloads[i]) == 1) {
			return payloads[i];
		}
	}
	
	return NULL;
}

void main() {
    FATFS fs;
    FIL payload_file;
    unsigned int br;

    f_mount(&fs, "0:", 1);

	char *payload = checkPayloads();
	if (payload == NULL)
		whiteScreen();

	if(f_open(&payload_file, payload, FA_READ) == FR_OK) {
		f_read(&payload_file, (void *)PAYLOAD_ADDRESS, f_size(&payload_file), &br);
		((void (*)())PAYLOAD_ADDRESS)();
    }

	else whiteScreen();
}
