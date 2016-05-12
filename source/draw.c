#include "common.h"

#define max(a, b) (((a)>(b)) ? (a) : (b))

#define fb_sz(fb) \
	(fb == framebuffers->bottom ? BOTTOM_FB_SZ : TOP_FB_SZ)

u8 rate = 0x0F; // Default, overridden by config (15 in decimal, for those of you who can't HEX)

size_t top_anim_size, bottom_anim_size, config_size;

struct framebuffer_t
{ // Thanks to mid-kid & CakesFW for fb offsets
    u8 *top_left;
    u8 *top_right;
    u8 *bottom;
};

struct framebuffer_t* framebuffers = (struct framebuffer_t *) 0x23FFFE00;

void clear_screen(u8* fb, u32 rgb)
{
    u32 fb_len = fb_sz(fb), i;
    while (i < fb_len)
    {
        *(u8*)(fb + i++) = rgb & 0xFF;
        *(u8*)(fb + i++) = (rgb >> 8) & 0xFF;
        *(u8*)(fb + i++) = (rgb >> 16) & 0xFF;
    }
    return;
}

u32 get_read_delay()
{
    FIL test_file;
	FRESULT f_ret;
    size_t br = 0, read_delay = 0;
	const char *test_fname = "/anim/calibrator";

	REG_TM0CNT = 0b10000111;
	REG_TM0VAL = 0; // Start the timer
	f_ret = f_open(&test_file, test_fname, FA_READ);
	if (f_ret != FR_OK)
		return 0;

	f_ret = f_read(&test_file, framebuffers->top_left, TOP_FB_SZ, &br);
	f_ret = f_read(&test_file, framebuffers->bottom, BOTTOM_FB_SZ, &br);
	f_close(&test_file);
	
	read_delay = REG_TM0VAL; // Get the timer's value
	REG_TM0CNT = 0b00000111; // Stop the timer
    REG_TM0VAL = 0; // Reset the timer to 0

	return read_delay;
}

void load_animation(u32 max)
{
    u32 rand_animation = REG_PRNG % (max); // Dice roll, with possible values between 0 and (maxAnimations-1)

    // Load from sdmc:/anim/(rand_animation)/{anim, bottom_anim, config}
    char *config_fname  = "/anim/0/config";
    char *top_fname     = "/anim/0/anim";
    char *bottom_fname  = "/anim/0/bottom_anim";

    config_fname[6] 	= rand_animation + '0'; // Yes, I'm directly modifying the '0' in the string
    top_fname[6]   		= rand_animation + '0'; // tfw no sprintf
    bottom_fname[6] 	= rand_animation + '0'; // And strcat just craps itself

    config_size			= file_exists(config_fname); // Get config file size (mostly to check whether it exists, because rn it only reads 1 byte)
    top_anim_size 		= file_size(top_fname);      // Get top screen animation size
    bottom_anim_size 	= file_size(bottom_fname);   // Get bottom screen animation size

    if (config_size)
	{
        FIL config_fil;
        unsigned int br;
        f_open(&config_fil, config_fname, FA_READ);
        f_read(&config_fil, &rate, 1, &br);
        f_close(&config_fil);
    }

    animation_loop(top_fname, bottom_fname, top_anim_size / TOP_FB_SZ, bottom_anim_size / BOTTOM_FB_SZ, rate); // Main animation loop, with filenames and amount of frames and stuff
    return;
}

/*void delay(u32 n)
{
    u32 i = n;
    while (i--)
	{
        __asm("mov r0, r0"); // Just a little delay
	}
}*/ // LEGACY DELAY FUNCTION, TERRIBLY BAD STUFF
// ONLY HERE TO SHOW KIDS HOW NOT TO CODE

// Thanks to hartmannaf for letting me know about 3dbrew.org/wiki/TIMER_Registers
inline void delay(u32 n)
{
	REG_TM0CNT = 0b10000111;
	REG_TM0VAL = 0;
    while(REG_TM0VAL < n)
        ;

    REG_TM0VAL = 0;
	REG_TM0CNT = 0b00000111;
}

void animation_loop(char *top_anim, char *bottom_anim, u32 top_frames, u32 bottom_frames, u8 frame_rate)
{
    clear_screen(framebuffers->top_left, 0x00);
    clear_screen(framebuffers->bottom, 0x00);

    FIL bgr_anim_bot, bgr_anim_top;
    unsigned int put_bot = 0, put_top = 0;

    if (top_frames) // If top animation exists
        f_open(&bgr_anim_top, top_anim, FA_READ);

    if (bottom_frames) // If bottom animation exists
        f_open(&bgr_anim_bot, bottom_anim, FA_READ);

    u32 max_frames = max(top_frames, bottom_frames); // Get the maximum amount of frames between the two animations

	u32 delay_ = 65456 / frame_rate;
	size_t read_delay = get_read_delay();
	if (read_delay >= delay_)
		delay_ = 0;

	else
		delay_ -= read_delay;

    u32 delay__ = (delay_ >> 1);

    for (u32 curframe = 0; curframe < max_frames; curframe++)
	{
        if (HID_PAD & (KEY_SELECT | KEY_START)) // End the animation if the 'SELECT' or 'START' key is being pressed
            curframe = max_frames;

        if (curframe <= top_frames) // If top animation hasn't ended yet
        {
            f_read(&bgr_anim_top, framebuffers->top_left, TOP_FB_SZ, &put_top); // Write to the framebuffer directly

            if (curframe <= bottom_frames)
                delay(delay__); // Half the delay

            else
                delay(delay_); // Whole delay
        }

        if (curframe < bottom_frames) // If bottom animation hasn't ended yet
        {
            f_read(&bgr_anim_bot, framebuffers->bottom, BOTTOM_FB_SZ, &put_bot); // Write to the framebuffer directly

            if (curframe <= top_frames) // check whether the top animation is playing
                delay(delay__); // Half the delay

            else
                delay(delay_); // Whole delay
        }
    }

    if (top_frames) // If top animation was opened, close it
        f_close(&bgr_anim_top);

    if (bottom_frames) // If bottom animation was opened, close it
        f_close(&bgr_anim_bot);

    clear_screen(framebuffers->top_left, 0x000000);
    clear_screen(framebuffers->bottom, 0x000000);

    return;
}
