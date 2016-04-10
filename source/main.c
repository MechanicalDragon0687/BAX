#include "main.h"

#include "types.h"
#include "draw.h"
#include "memory.h"
#include "fatfs/ff.h"
#include "fs.h"

#include "loader.h"

static FATFS fs;

void chainload() { // Load and execute the chainloader
	memcpy((void*)PAYLOAD_ADDR, loader_bin, loader_bin_len);
	((void(*)(void))PAYLOAD_ADDR)();
}

u32 checkFiles() {
	char   *top    = "/anim/0/anim",
	       *bottom = "/anim/0/bottom_anim";

	u32 anims_amount = 0;

	for (u32 i = 0; i < 10; i++) { // Check files from dir '0' to '9'
		top[6]    = i + '0';
		bottom[6] = i + '0';

		if (fileExists(top) + fileExists(bottom) > 0) // If at least one of them exists, then add 1
			anims_amount++;

		else break; // Otherwise, break the loop
	}

	return anims_amount;
}

void main() {

	if (f_mount(&fs, "0:", 1) != FR_OK) // Mount the SD card
		chainload(); // Try to chainload if mounting fails, shouldn't work but you never know ;)

	u32 anims = checkFiles();

	if (!anims) // If anims == 0, then there are no animations
		chainload(); // Just chainload, skip the animation entirely

	if (!(*(vu8 *)0x10010000) & !(HID_PAD & BUTTON_R1)) // Check if this is a coldboot and if R trigger is pressed
		loadAnimation(anims); // Load animations, with (anims) amount of animations

	chainload(); // When it finishes, chainload

	return;
}
