#include "common.h"

#define max(a, b) (((a)>(b)) ? (a) : (b))

#define fb_sz(fb) \
    (fb == framebuffers->bottom ? SUB_FB_SZ : TOP_FB_SZ)

char cfg[2] = {0};
// cfg[0] is framerate, cfg[1] is the compression flag

struct framebuffer_t
{ // Thanks to mid-kid & CakesFW for fb offsets
    u8 *top_left;
    u8 *top_right;
    u8 *bottom;
};

struct framebuffer_t* framebuffers = (struct framebuffer_t *) 0x23FFFE00;

void clear_screen(u8* fb, u32 rgb)
{
    size_t fb_len = fb_sz(fb), i;
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
    size_t br, read_delay;
    const char *test_fname = "/anim/calibrator";

    REG_TM0CNT = 0b10000111;
    REG_TM0VAL = 0; // Start the timer
    f_ret = f_open(&test_file, test_fname, FA_READ);
    if (f_ret != FR_OK)
        return 0;

    f_ret = f_read(&test_file, framebuffers->top_left, TOP_FB_SZ, &br);
    f_ret = f_read(&test_file, framebuffers->bottom, SUB_FB_SZ, &br);
    f_close(&test_file);

    read_delay = REG_TM0VAL; // Get the timers value
    REG_TM0CNT = 0b00000111; // Stop the timer
    REG_TM0VAL = 0; // Reset the timer to 0

    return read_delay;
}

void load_animation(u32 max)
{
    u32 rngesus = REG_PRNG % (max); // Dice roll, with possible values between 0 and (maxAnimations-1)

    // Load from sdmc:/anim/(rngesus)/{anim, bottom_anim, config}
    char *config_fname  = "/anim/0/config";
    char *top_fname     = "/anim/0/anim";
    char *bottom_fname  = "/anim/0/bottom_anim";

    config_fname[6]     = rngesus + '0'; // Yes, I'm directly modifying the '0' in the string
    top_fname[6]   	    = rngesus + '0';
    bottom_fname[6]     = rngesus + '0';

    FIL config_fil;
    size_t br = 0;
    if (f_open(&config_fil, config_fname, FA_READ) == FR_OK)
    {
        f_read(&config_fil, &cfg, 2, &br);
        f_close(&config_fil);
    }

    if (cfg[0] < 1)
        cfg[0] = 30; // Let's avoid division by zero/negatives, shall we?

    animation_loop(top_fname, bottom_fname, cfg[0], cfg[1]);

    clear_screen(framebuffers->top_left, 0x00);
    clear_screen(framebuffers->bottom, 0x00);

    return;
}

// Thanks to hartmannaf for letting me know about 3dbrew.org/wiki/TIMER_Registers
inline void delay(u32 n)
{
    REG_TM0CNT = 0b10000111;
    REG_TM0VAL = 0;
    while(REG_TM0VAL < n);

    REG_TM0VAL = 0;
    REG_TM0CNT = 0b00000111;
}

void animation_loop(char *top_anim, char *bottom_anim, const char frame_rate, const char compressed)
{
    u32 delay_ = 65456 / frame_rate, read_delay = get_read_delay();
    FIL bgr_anim_bot, bgr_anim_top;
    unsigned int put_bot = 0, put_top = 0;
    char top_anim_flag = 0, sub_anim_flag = 0;

    if (f_open(&bgr_anim_top, top_anim, FA_READ) == FR_OK) // If top animation can be opened
        top_anim_flag = 1;

    if (f_open(&bgr_anim_bot, bottom_anim, FA_READ) == FR_OK) // If bottom animation can be opened
        sub_anim_flag = 1;

    if (delay_ < read_delay) // Underflows are not nice...
        delay_ = 0;

    else
        delay_ -= read_delay;

    u32 delay__ = (delay_ >> 1); // THIS IS HORRIBLY INCORRECT AND MUST BE FIXED ASAP

	if (compressed == 0) // Regular animation playback
	{
		while(1)
		{
            if (HID_PAD & (KEY_SELECT | KEY_START)) // End the animation if the 'SELECT' or 'START' key is pressed
                break;

            if (top_anim_flag)
			{
				f_read(&bgr_anim_top, framebuffers->top_left, TOP_FB_SZ, &put_top);
				if (put_top != TOP_FB_SZ)
					break; // Reached EOF
			}

			if (sub_anim_flag)
			{
				f_read(&bgr_anim_bot, framebuffers->bottom, SUB_FB_SZ, &put_bot);
				if (put_bot != SUB_FB_SZ)
					break; // Reached EOF
			}

            if (top_anim_flag && sub_anim_flag)
                delay(delay__);

            else
                delay(delay_);

		}
	}
	
    else if (compressed == 1) // Compressed animation playback
    {
        char top_frame_prev[TOP_FB_SZ], top_frame_curr[TOP_FB_SZ], top_frame_comp[TOP_FB_SZ + 9],
             sub_frame_prev[SUB_FB_SZ], sub_frame_curr[SUB_FB_SZ], sub_frame_comp[SUB_FB_SZ + 9];

		// An extra 9 bytes, just to be safe
        // After some brain was applied, the max frame size is (original + 9), due to each frame having a QuickLZ header

        // Clear buffers
        memset(top_frame_prev, 0, TOP_FB_SZ);
        memset(top_frame_curr, 0, TOP_FB_SZ);
        memset(top_frame_comp, 0, TOP_FB_SZ + 9);
        memset(sub_frame_prev, 0, SUB_FB_SZ);
        memset(sub_frame_curr, 0, SUB_FB_SZ);
        memset(sub_frame_comp, 0, SUB_FB_SZ + 9);

        qlz_state_decompress *state_decompress = (qlz_state_decompress*)0x24F00000; // This region should be clear
        memset(state_decompress, 0, sizeof(qlz_state_compress));

        size_t comp_size = 0;

        while(1)
        {
            if (HID_PAD & (KEY_SELECT | KEY_START)) // End the animation if the 'SELECT' or 'START' key is pressed
                break;

            if (top_anim_flag) // If there's a top animation...
            {
                if (f_read(&bgr_anim_top, (void*)top_frame_comp, 9, &put_top) != FR_OK)
                    break;

                comp_size = qlz_size_compressed(top_frame_comp);

                f_read(&bgr_anim_top, (void*)top_frame_comp + 9, comp_size - 9, &put_top);

                if (qlz_size_decompressed(top_frame_comp) != TOP_FB_SZ || put_top != (comp_size - 9))
                    break;

                // Decompress the frame
                qlz_decompress(top_frame_comp, top_frame_curr, (qlz_state_decompress*)0x24F00000);

                // Delta decoding
                for (u32 i = 0; i < TOP_FB_SZ; i++)
                    top_frame_curr[i] += top_frame_prev[i];

                // Keep the current frame in memory
                memcpy(top_frame_prev, top_frame_curr, TOP_FB_SZ);

                // Copy to framebuffer
                memcpy(framebuffers->top_left, top_frame_curr, TOP_FB_SZ);
            }

            if (sub_anim_flag) // If there's a bottom animation...
            {
                if (f_read(&bgr_anim_bot, (void*)sub_frame_comp, 9, &put_bot) != FR_OK)
                    break;

                comp_size = qlz_size_compressed(sub_frame_comp);

                f_read(&bgr_anim_bot, (void*)sub_frame_comp + 9, comp_size - 9, &put_bot);

                if (qlz_size_decompressed(sub_frame_comp) != SUB_FB_SZ || put_bot != (comp_size - 9))
                    break;

                qlz_decompress(sub_frame_comp, sub_frame_curr, (qlz_state_decompress*)0x24F00000);

                for (u32 i = 0; i < SUB_FB_SZ; i++)
                    sub_frame_curr[i] += sub_frame_prev[i];

                // Keep the current frame in memory
                memcpy(sub_frame_prev, sub_frame_curr, SUB_FB_SZ);

                // Copy to framebuffer
                memcpy(framebuffers->bottom, sub_frame_curr, SUB_FB_SZ);
            }

            if (top_anim_flag && sub_anim_flag)
                delay(delay__);

            else
                delay(delay_);
        }
    }
    if (top_anim_flag) // If top animation was opened, close it
        f_close(&bgr_anim_top);

    if (sub_anim_flag) // If bottom animation was opened, close it
        f_close(&bgr_anim_bot);

    return;
}
