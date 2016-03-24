#include "draw.h"
#include "memory.h"

#define MAX_SIZE	(50*1024*1024)
#define LOAD_ADDR	0x24000000

#include "fs.h"

struct framebuffers { // thsnks to mid-kid for fb offsets
    u8 *top_left;
    u8 *top_right;
    u8 *bottom;
} *framebuffers = (struct framebuffers *) 0x23FFFE00;

u32 fb_sz(u8* fb) {
	if (fb == framebuffers->top_left || fb == framebuffers->top_right)
		return TOP_FB_SZ;
	else return BOTTOM_FB_SZ;
}

void clearScreen() {
	memset(framebuffers->top_left, 0xFF, TOP_FB_SZ);
	memset(framebuffers->bottom, 0xFF, BOTTOM_FB_SZ);
}

u32 max(u32 n_1, u32 n_2) {
	if (n_1 > n_2) return n_1;
	else return n_2;
}

void drawBootScreen() {
	clearScreen(); // clear the screen
	char top_anim[] = "/anim", bottom_anim[] = "/bottom_anim"; // define file names
	u32 topAnimSize, topFrames, bottomAnimSize, bottomFrames; // define variables

	topAnimSize = fileSize(top_anim); // get top screen animation size
	bottomAnimSize = fileSize(bottom_anim); // get bottom screen animation size

	if ((topAnimSize + bottomAnimSize) >= MAX_SIZE || (topAnimSize+bottomAnimSize) == 0) // return if higher than 64MB or non-existant
		return;

	topFrames = (topAnimSize / TOP_FB_SZ); // get top screen frames
	bottomFrames = (bottomAnimSize / BOTTOM_FB_SZ); // get bottom screen frames

	fileRead((u8*)LOAD_ADDR, top_anim, topAnimSize); // read the top animation to LOAD_ADDR
	fileRead((u8*)LOAD_ADDR + topAnimSize, bottom_anim, bottomAnimSize); // read the bottom animation to (LOAD_ADDR + topAnimSize)

	u32 frames = max(topFrames, bottomFrames); // get the maximum amount of frames between the two animations

	for (u32 curframe = 0; curframe < frames; curframe++) { // loop until the maximum amount of frames, increasing frame count by 1

		if (topAnimSize != 0 && curframe < topFrames) // if top animation exists and hasn't ended yet
			memcpy((void*)framebuffers->top_left, (void*)LOAD_ADDR + (curframe*TOP_FB_SZ), TOP_FB_SZ); // refresh top animation

		if (bottomAnimSize != 0 && curframe < bottomFrames) // if bottom animation exists and hasn't ended yet
			memcpy((void*)framebuffers->bottom, (void*)LOAD_ADDR + topAnimSize + (curframe*BOTTOM_FB_SZ), BOTTOM_FB_SZ); // refresh bottom animation

		u32 i = 0x1AAAAA; while (i--) __asm("andeq r0, r0, r0"); // ASM NOP(e), supposed to delay animation
		// THIS HAS BEEN CALIBRATED FOR 10 FPS
	}
	return;
}
