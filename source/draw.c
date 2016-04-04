#include "draw.h"

#include "main.h"
#include "memory.h"
#include "fatfs/ff.h"

#define max(a,b) ({a > b ? a :  b; })

#define delay(n) ({u32 i = n; while (i--) __asm("mov r0, #0");}) // ASM NOP(e), supposed to delay animation*

struct framebuffer_t { // Thanks to mid-kid for fb offsets
    u8 *top_left;
    u8 *top_right;
    u8 *bottom;
};

struct framebuffer_t* framebuffers = (struct framebuffer_t *) 0x23FFFE00;

u32 fb_sz(u8* fb) {
	if (fb == framebuffers->bottom)
		return BOTTOM_FB_SZ;
	else return TOP_FB_SZ;
}

void clearScreen() {
	memset(framebuffers->top_left, 0x00, TOP_FB_SZ);
	memset(framebuffers->bottom,   0x00, BOTTOM_FB_SZ);
}

u32 fileSize(const char * path) {
	FIL file;
	u32 size = 0;
	
	if (f_open(&file, path, FA_READ) == FR_OK)
		size = f_size(&file);

	return size;
}

void animationLoop() {
	clearScreen(); // Clear to black, just in case

	char *config      = "/anim/config";
	char *top_anim    = "/anim/anim";
	char *bottom_anim = "/anim/bottom_anim"; // Define file names

	u8 rate = 15; // Default, overridden by config

	u32	topAnimSize = 0, topFrames = 0,	bottomAnimSize = 0,	bottomFrames = 0;

	topAnimSize 	= fileSize(top_anim);       // Get top screen animation size
	bottomAnimSize 	= fileSize(bottom_anim); 	// Get bottom screen animation size
	u32 configSize 	= fileSize(config);

	if (topAnimSize == 0 && bottomAnimSize == 0) return; // No animation, just chain already

	// No more 64MB check since we directly read to the framebuffer. It can be unbounded.
	if (topAnimSize)
		topFrames    = ((topAnimSize    - 1) / TOP_FB_SZ);    // get top screen frames
	if (bottomAnimSize)
		bottomFrames = ((bottomAnimSize - 1) / BOTTOM_FB_SZ); // get bottom screen frames

	// Read the config if it exists, otherwise default to 15fps
	if (configSize)	{
		unsigned int br = 0; // Bytes read, not really useful FWIW
		FIL config_fil;
		f_open(&config_fil, config, FA_READ); // Open the config file...
		f_read(&config_fil, &rate, 1, &br); // Read it...
	} // Never close it due to a bug

	FIL bgr_anim_bot, bgr_anim_top;
	unsigned int br_top, br_bot; // Bytes read, top and bottom

	if (topFrames > 0) {
		f_open(&bgr_anim_top, top_anim, FA_READ);
		br_top = 0;
	}

	if (bottomFrames > 0) {
		f_open(&bgr_anim_bot, bottom_anim, FA_READ);
		br_bot = 0;
	}

	u32 maxFrames = max(topFrames, bottomFrames); // Get the maximum amount of frames between the two animations

	u32 delay_ = 0;

	if (rate < 25)
		delay_ = (6990480 / rate); // Need to take more accurate measurements, but this will do, it's quite a magic number

	u32 delay__ = (delay_ / 2); // FIXME - THIS IS NOT OKAY. Hey, it's just a bad approximation, M'kay?

	for (u32 curframe = 0; curframe < maxFrames; curframe++) { // loop until the maximum amount of frames, increasing frame count by 1

		if (HID_PAD & BUTTON_SELECT) // End the animation if the 'SELECT' key is pressed
			curframe = maxFrames;

		if (topAnimSize != 0 && curframe < topFrames) { // If top animation exists and hasn't ended yet
			f_read(&bgr_anim_top, framebuffers->top_left, TOP_FB_SZ, &br_top); // AKA write to the framebuffer directly.

			if (curframe <= bottomFrames)
				delay(delay__);
			else
				delay(delay_);
		}

		if (bottomAnimSize != 0 && curframe < bottomFrames) { // If bottom animation exists and hasn't ended yet
			f_read(&bgr_anim_bot, framebuffers->bottom, BOTTOM_FB_SZ, &br_bot); // AKA write to the framebuffer directly.

			 if (curframe <= topFrames) // Check whether the top animation is playing
  				delay(delay__); // Half the delay
  			else
  				delay(delay_); // Whole delay
		}
		// THIS HAS BEEN PARTIALLY CALIBRATED
	}

	clearScreen(); // Just in case we boot a payload that isn't nice enough to clean up :)

	// No, I did not forget to f_close() the files. This (somehow) caused a bug that has been fixed by removing these calls
}
