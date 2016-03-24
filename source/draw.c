#include "draw.h"
#include "memory.h"
#include "fatfs/ff.h"
#include "types.h"

#define LOAD_ADDR	0x24000000

#include "fs.h"

u8 rate = 15; // Default, overridden by config

u32	topAnimSize,
	topFrames,
	bottomAnimSize,
	bottomFrames,
	delay_,
	delay__;

char *config      = "/anim/config";
char *top_anim    = "/anim/anim";
char *bottom_anim = "/anim/anim_bottom"; // define file names


struct framebuffer_t { // thsnks to mid-kid for fb offsets
    u8 *top_left;
    u8 *top_right;
    u8 *bottom;
};
struct framebuffer_t* framebuffers = (struct framebuffer_t *) 0x23FFFE00;

u32 fb_sz(u8* fb) {
	if (fb == framebuffers->top_left || fb == framebuffers->top_right)
		return TOP_FB_SZ;
	else return BOTTOM_FB_SZ;
}

void clearScreen() {
	memset(framebuffers->top_left, 0x00, TOP_FB_SZ);
	memset(framebuffers->bottom,   0x00, BOTTOM_FB_SZ);
}

u32 max(u32 n_1, u32 n_2) { // Just because I don't like <math.h> :D
	if (n_1 > n_2) return n_1;
	else return n_2;
}

void delay(u32 n) {
	u32 i = n; while (i--) __asm("andeq r0, r0, r0"); // ASM NOP(e), supposed to delay animation
}

u32 loadFiles() {
	topAnimSize 	= fileSize(top_anim);       // get top screen animation size
	bottomAnimSize 	= fileSize(bottom_anim); // get bottom screen animation size
	u32 configSize 	= fileSize(config);

	if (topAnimSize == 0 && bottomAnimSize == 0) return 0; // No animation, just chain already

	// No more 64MB check since we directly read to the framebuffer. It can be unbounded.
	topFrames    = (topAnimSize		/	TOP_FB_SZ);    // get top screen frames
	bottomFrames = (bottomAnimSize	/	BOTTOM_FB_SZ); // get bottom screen frames

	// Read the config if it exists, otherwise default to 15fps
	if (fileExists(config)) {
		fileRead(&rate, config, configSize);
	}
	
	return 1;
}

void multiplayDelay(u32 curframe, u32 maxPossibleFrames) {
	if (curframe <= maxPossibleFrames) // If it's still playing
		delay(delay__);
	else delay(delay_);
}

void animationLoop() {
	clearScreen(); // Clear to black
	
	if (!loadFiles()) return;

	FIL bgr_anim_bot, bgr_anim_top;
	unsigned int put_bot, put_top;
	int ret = 0;

	if (topFrames > 0) {
		ret = f_open(&bgr_anim_top, top_anim, FA_READ);
		put_top = 0;
	}

	if (bottomFrames > 0) {
		ret = f_open(&bgr_anim_bot, bottom_anim, FA_READ);
		put_bot = 0;
	}

	u32 maxFrames = max(topFrames, bottomFrames); // Get the maximum amount of frames between the two animations
	delay_ = (6990480 / rate); // Need to take more accurate measurements, but this will do, it's quite a magic number
	delay__ = (delay_ / 2); // FIXME - THIS IS NOT OKAY. Hey, it's just a bad approximation, M'kay?

	for (u32 curframe = 0; curframe < maxFrames; curframe++) { // loop until the maximum amount of frames, increasing frame count by 1
		if (topAnimSize != 0 && curframe < topFrames) { // if top animation exists and hasn't ended yet
			f_read(&bgr_anim_top, framebuffers->top_left, TOP_FB_SZ, &put_top); // AKA Read to the framebuffer directly.

			multiplayDelay(curframe, bottomFrames);
		}

		if (bottomAnimSize != 0 && curframe < bottomFrames) { // if bottom animation exists and hasn't ended yet
			f_read(&bgr_anim_bot, framebuffers->bottom, BOTTOM_FB_SZ, &put_bot); // AKA Read to the framebuffer directly.

			multiplayDelay(curframe, topFrames);
		}
		// THIS HAS BEEN PARTIALLY CALIBRATED
	}

	f_close(&bgr_anim_bot);
	f_close(&bgr_anim_top);
}
