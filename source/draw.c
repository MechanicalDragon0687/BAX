#include "common.h"

char cfg[2] = {0x00};
// cfg[0] is framerate, cfg[1] is the compression flag

const size_t TOP_FB_SZ = 288000; // 400 * 240 * 3
const size_t SUB_FB_SZ = 230400; // 320 * 240 * 3

framebuffer_t *framebuffer = (framebuffer_t *)0x23FFFE00;

inline size_t fb_sz(u8 *fb)
{
    if (fb == framebuffer->bottom)
        return SUB_FB_SZ;

    else if (fb == framebuffer->top_left || fb == framebuffer->top_right)
        return TOP_FB_SZ;

    else return 0;
}

void clear_screen(u8 *fb, u32 rgb)
{
    size_t fb_len = fb_sz(fb), i = 0;
    while (i < fb_len)
    {
        *(u8*)(fb + i++) = rgb & 0xFF;
        *(u8*)(fb + i++) = (rgb >> 8) & 0xFF;
        *(u8*)(fb + i++) = (rgb >> 16) & 0xFF;
    }
    return;
}

size_t get_read_delay()
{
    FIL test_file;
    FRESULT f_ret;
    size_t br, read_delay;
    const char *test_fname = "/anim/calibrator";

    REG_TM0CNT = 0b10000111;
    REG_TM0VAL = 0; // Start the timer
    f_ret = f_open(&test_file, test_fname, FA_READ);
    if (f_ret != FR_OK)
        return 2182; // (65457 / 30), so run the thing as fast as possible

    f_read(&test_file, framebuffer->top_left, TOP_FB_SZ, &br);
    f_read(&test_file, framebuffer->bottom, SUB_FB_SZ, &br);
    f_close(&test_file);

    read_delay = REG_TM0VAL; // Get the timers value
    REG_TM0CNT = 0b00000111; // Stop the timer
    REG_TM0VAL = 0; // Reset the timer to 0

    return read_delay;
}

void load_animation(int max)
{
    int rngesus = REG_PRNG % max; // Dice roll

    // Load from /anim/'<rngesus>'/{anim, bottom_anim, config}
    char *cfg_fname  = "/anim/0/config";
    char *top_fname  = "/anim/0/anim";
    char *sub_fname  = "/anim/0/bottom_anim";

    memset(&cfg_fname[6], rngesus + '0', 1);
    memset(&top_fname[6], rngesus + '0', 1);
    memset(&sub_fname[6], rngesus + '0', 1);

    FIL config_fil;
    size_t br = 0;
    if (f_open(&config_fil, cfg_fname, FA_READ) == FR_OK)
    {
        f_read(&config_fil, &cfg, 2, &br);
        f_close(&config_fil);
    }

    if (cfg[0] < 1)
        cfg[0] = 15; // Let's avoid division by zero/negatives, shall we?

    animation_loop(top_fname, sub_fname, cfg[0], cfg[1]);

    clear_screen(framebuffer->top_left, 0x00);
    clear_screen(framebuffer->bottom, 0x00);

    return;
}

// Thanks to hartmannaf for letting me know about 3dbrew.org/wiki/TIMER_Registers
inline void delay(size_t n)
{
    REG_TM0VAL = 0;
    REG_TM0CNT = 0b10000111;

    while(REG_TM0VAL < n);

    REG_TM0VAL = 0;
    REG_TM0CNT = 0b00000111;
}

void animation_loop(char *top_anim, char *bottom_anim, const char frame_rate, const char compressed)
{
    size_t delay_ = (65457 / frame_rate), read_delay = get_read_delay(), put_bot = 0, put_top = 0;
    FIL bgr_anim_bot, bgr_anim_top;
    char top_anim_flag = 0, sub_anim_flag = 0;

    if (f_open(&bgr_anim_top, top_anim, FA_READ) == FR_OK) // If top animation opened fine
        top_anim_flag = 1;

    if (f_open(&bgr_anim_bot, bottom_anim, FA_READ) == FR_OK) // If bottom animation opened fine
        sub_anim_flag = 1;

    if (delay_ < read_delay) // Underflows are not nice...
        delay_ = 0;

    else
        delay_ -= read_delay;

    size_t delay_top_only = ((delay_ * 240) / 400), delay_sub_only = ((delay_ * 240) / 320);

	if (compressed == 0) // Regular animation playback
	{
		while(1)
		{
            if (HID_PAD & (KEY_SELECT | KEY_START)) // End the animation if the 'SELECT' or 'START' key is pressed
                break;

            if (top_anim_flag)
            {
                f_read(&bgr_anim_top, framebuffer->top_left, TOP_FB_SZ, &put_top);
                if (put_top != TOP_FB_SZ)
                {
                    top_anim_flag = 0;
                }
            }

            if (sub_anim_flag)
            {
                f_read(&bgr_anim_bot, framebuffer->bottom, SUB_FB_SZ, &put_bot);
                if (put_bot != SUB_FB_SZ)
                {
                    sub_anim_flag = 0;
                }
            }

            if (put_top != TOP_FB_SZ && put_bot != SUB_FB_SZ)
                break;

            if (top_anim_flag && sub_anim_flag)
                delay(delay_);

            else if (top_anim_flag)
                delay(delay_top_only);

            else if (sub_anim_flag)
                delay(delay_sub_only);
        }
    }

    else if (compressed == 1) // Compressed animation playback
    {
        char top_frame_prev[TOP_FB_SZ], top_frame_curr[TOP_FB_SZ], top_frame_comp[TOP_FB_SZ + (TOP_FB_SZ/10) + 9],
             sub_frame_prev[SUB_FB_SZ], sub_frame_curr[SUB_FB_SZ], sub_frame_comp[SUB_FB_SZ + (SUB_FB_SZ/10) + 9];

        // https://sourceforge.net/p/sevenzip/discussion/45797/thread/b6bd62f8/#4a3d
        // Given TOP_FB_SZ/SUB_FB_SZ are so (relatively) small, it makes no sense to use (+ 64K)

        // Clear buffers
        memset(top_frame_prev, 0, TOP_FB_SZ);
        memset(top_frame_curr, 0, TOP_FB_SZ);
        memset(top_frame_comp, 0, TOP_FB_SZ + (TOP_FB_SZ/10) + 9);
        memset(sub_frame_prev, 0, SUB_FB_SZ);
        memset(sub_frame_curr, 0, SUB_FB_SZ);
        memset(sub_frame_comp, 0, SUB_FB_SZ + (SUB_FB_SZ/10) + 9);

        qlz_state_decompress *state_decompress = (qlz_state_decompress*)0x25F00000; // Hopefully there's nothing too interesting here :^)
        memset(state_decompress, 0, sizeof(qlz_state_compress));

        size_t comp_size = 0;

        while(1)
        {
            if (HID_PAD & (KEY_SELECT | KEY_START)) // End the animation if the 'SELECT' or 'START' key is pressed
                break;

            if (top_anim_flag) // If there's a top animation...
            {
                f_read(&bgr_anim_top, (void*)top_frame_comp, 9, &put_top);
                if (put_top != 9)
                {
                    top_anim_flag = 0;
                    goto skip_1;
                }

                comp_size = qlz_size_compressed(top_frame_comp);

                f_read(&bgr_anim_top, (void*)top_frame_comp + 9, comp_size - 9, &put_top);

                if (qlz_size_decompressed(top_frame_comp) != TOP_FB_SZ || put_top != (comp_size - 9))
                    break;

                qlz_decompress(top_frame_comp, top_frame_curr, (qlz_state_decompress*)0x25F00000); // Hopefully there's nothing here...

                for (size_t i = 0; i < TOP_FB_SZ; i++)
                    top_frame_curr[i] += top_frame_prev[i];

                memcpy(top_frame_prev, top_frame_curr, TOP_FB_SZ);
                memcpy(framebuffer->top_left, top_frame_curr, TOP_FB_SZ);
            }

            if (sub_anim_flag) // If there's a bottom animation...
            {
                f_read(&bgr_anim_bot, (void*)sub_frame_comp, 9, &put_bot);
                if (put_bot != 9)
                {
                    sub_anim_flag = 0;
                    goto skip_1;
                }

                comp_size = qlz_size_compressed(sub_frame_comp);

                f_read(&bgr_anim_bot, (void*)sub_frame_comp + 9, comp_size - 9, &put_bot);

                if (qlz_size_decompressed(sub_frame_comp) != SUB_FB_SZ || put_bot != (comp_size - 9))
                    break;

                qlz_decompress(sub_frame_comp, sub_frame_curr, (qlz_state_decompress*)0x24F00000);

                for (size_t i = 0; i < SUB_FB_SZ; i++)
                    sub_frame_curr[i] += sub_frame_prev[i];

                memcpy(sub_frame_prev, sub_frame_curr, SUB_FB_SZ);
                memcpy(framebuffer->bottom, sub_frame_curr, SUB_FB_SZ);
            }

            skip_1: // Wynaut?

            if (top_anim_flag != 1 && sub_anim_flag != 1)
                break;

            if (top_anim_flag && sub_anim_flag)
                delay(delay_);

            else if (top_anim_flag)
                delay(delay_top_only);

            else if (sub_anim_flag)
                delay(delay_sub_only);
        }
    }

    if (top_anim_flag) // If top animation was opened, close it
        f_close(&bgr_anim_top);

    if (sub_anim_flag) // If bottom animation was opened, close it
        f_close(&bgr_anim_bot);

    return;
}
