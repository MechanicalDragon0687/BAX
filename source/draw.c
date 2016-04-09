#include "main.h"
#include "draw.h"
#include "memory.h"
#include "fatfs/ff.h"
#include "types.h"
#include "fs.h"

#define max(a, b) (((a)>(b))?(a) : (b))

u8 rate = 0x0F; // Default, overridden by config (15 in decimal, for those of you who can't hex)

u32	topAnimSize = 0, bottomAnimSize = 0, configSize = 0; // Define filesizes, 0 by default

struct framebuffer_t { // Thanks to mid-kid & CakesFW for fb offsets
    u8 *top_left;
    u8 *top_right;
    u8 *bottom;
};
struct framebuffer_t* framebuffers = (struct framebuffer_t *) 0x23FFFE00;

/*u32 fb_sz(u8* fb) {
	if (fb == framebuffers->bottom)
		return BOTTOM_FB_SZ;
	else return TOP_FB_SZ;
}*/ // I'll use this someday... probably

void clearScreen() {
	memset(framebuffers->top_left, 0x00, TOP_FB_SZ);
	memset(framebuffers->bottom,   0x00, BOTTOM_FB_SZ);
}

void loadAnimation(u32 maxAnimations) {

	u32 rand_animation = REG_PRNG % (maxAnimations); // Dice roll, with possible values between 0 and (maxAnimations-1)

	// Load from sdmc:/anim/(rand_animation)/{anim, bottom_anim, config}
	char *config_fname = "/anim/0/config";
	char *top_fname    = "/anim/0/anim";
	char *bottom_fname = "/anim/0/bottom_anim";

	config_fname[6] = rand_animation + '0'; // Yes, I'm directly modifying the '0' in the string
	top_fname[6]    = rand_animation + '0'; // tfw no sprintf
	bottom_fname[6] = rand_animation + '0'; // And strcat just craps itself

	// I know the maximum theoterical limit of animations is 36
	// Due to the fact that I could do something like
	/* if (rand_animation > '0')
		rand_animation += 'a' - '0';*/
	// And I'll obtain animation directories ranging from '0' through '9'
	// And then the whole alphabet, from 'a' to 'z'
	// I won't though, 10 animations plenty, I mean who reboots their 3DS over 10 times a day?

	configSize		= fileSize(config_fname); // Get config file size (mostly to check whether it exists, because rn it only reads 1 byte)
	topAnimSize 	= fileSize(top_fname);    // Get top screen animation size
	bottomAnimSize 	= fileSize(bottom_fname); // Get bottom screen animation size

	if (configSize) {
		FIL config_fil;
		unsigned int br;
		f_open(&config_fil, config_fname, FA_READ);
		f_read(&config_fil, &rate, 1, &br);
		f_close(&config_fil);
	}

	animationLoop(top_fname, bottom_fname, topAnimSize / TOP_FB_SZ, bottomAnimSize / BOTTOM_FB_SZ, rate); // Main animation loop, with filenames and amount of frames and stuff
	return;
}

void delay(u32 n) {
	u32 i = n;
	while (i--)
		__asm("mov r0, #0"); // Just a little delay
}

void animationLoop(char *top_anim, char *bottom_anim, u32 topFrames, u32 bottomFrames, u8 frameRate) {
	// Only reason I have the framerate as an extra parameter is due to portability
	// Provided fatfs and framebuffers are available, this is pretty portable fwiw
	// Except the delay part ofc

	clearScreen(); // Clear both screens

	FIL bgr_anim_bot, bgr_anim_top;
	unsigned int put_bot, put_top; // Because FatFS

	if (topFrames > 0) { // If top animation exists
		f_open(&bgr_anim_top, top_anim, FA_READ);
		put_top = 0;
	}

	if (bottomFrames > 0) { // If bottom animation exists
		f_open(&bgr_anim_bot, bottom_anim, FA_READ);
		put_bot = 0;
	}

	u32 maxFrames = max(topFrames, bottomFrames); // Get the maximum amount of frames between the two animations

	u32 delay_ = 0; // Set delay to 0, for those using high framerate animations

	if (frameRate <= 24 && frameRate > 0) // If animation is (relatively) low fps...
		delay_ = (6990480 / frameRate); // Need to take more accurate measurements, but this will do, it's quite a magic number

	u32 delay__ = (delay_ / 2); // FIXME - THIS IS NOT OKAY. Hey, it's just a bad approximation, M'kay?

	for (u32 curframe = 0; curframe < maxFrames; curframe++) {

		if (HID_PAD & BUTTON_SELECT) // End the animation if the 'SELECT' key is pressed
			curframe = maxFrames;

		if (curframe <= topFrames) { // If top animation hasn't ended yet
			f_read(&bgr_anim_top, framebuffers->top_left, TOP_FB_SZ, &put_top); // Write to the framebuffer directly

			if (curframe <= bottomFrames)
				delay(delay__); // Half the delay
			else
				delay(delay_); // Whole delay
		}

		if (curframe < bottomFrames) { // If bottom animation hasn't ended yet
			f_read(&bgr_anim_bot, framebuffers->bottom, BOTTOM_FB_SZ, &put_bot); // Write to the framebuffer directly

			if (curframe <= topFrames) // check whether the top animation is playing
  				delay(delay__); // Half the delay
  			else
  				delay(delay_); // Whole delay
		}

		// THIS HAS BEEN PARTIALLY CALIBRATED
	}

	if (bottomFrames) // If bottom animation's FIL was opened, close
		f_close(&bgr_anim_bot);

	if (topFrames) // If top animation's FIL was opened, close
		f_close(&bgr_anim_top);
	
	return;
}
