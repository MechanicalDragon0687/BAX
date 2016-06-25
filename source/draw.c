#include "common.h"

u8 cfg[2] = {0}; // Current animations configuration
// cfg[0] is framerate, cfg[1] is the compression flag

u32 fb_sz(u8 *fb)
{
    if (fb == BOT_SCREEN0)
        return SUB_FB_SZ;

    else if (fb == TOP_SCREEN0 || fb == TOP_SCREEN1)
        return TOP_FB_SZ;

    else
        return 0; // This should never happen unless I'm an idiot or someone messes with the source/binary (or both :P)
}

void clear_screen(u8 *fb, u32 rgb)
{
    u32 fb_len = fb_sz(fb), i = 0;
    while (i < fb_len)
    {
        *(u8*)(fb + i++) = rgb & 0xFF;
        *(u8*)(fb + i++) = (rgb >> 8) & 0xFF;
        *(u8*)(fb + i++) = (rgb >> 16) & 0xFF;
    }

    return;
}

// Thanks to hartmannaf for letting me know about 3dbrew.org/wiki/TIMER_Registers
inline void delay(u32 n)
{
    if (n == 0) // In case delay is 0
        return;

    if (n > 65534)
        n = 65534;

    REG_TM0VAL = 0; // Reset the timer
    REG_TM0CNT = 0x87; // Start timer count

    while(REG_TM0VAL < n); // Delay n ticks

    REG_TM0CNT = 0x07; // Stop timer count
    REG_TM0VAL = 0; // Reset the timer
}

u32 get_read_delay()
{
    FIL test_file;
    FRESULT f_ret;
    size_t br_top, br_sub, read_delay;
    const char test_fname[] = "/anim/calibrator"; // Hardcoded, maybe I'll do something else in the future

    f_ret = f_open(&test_file, test_fname, FA_READ);
    if (f_ret != FR_OK)
    {
        // In case it doesn't exist, create it
        u8 tmp_buf[TOP_FB_SZ+SUB_FB_SZ] = {0};
        f_ret = f_open(&test_file, test_fname, FA_CREATE_ALWAYS | FA_WRITE);

        if (f_ret != FR_OK) // Well, this is awkward
            return 0;

        f_write(&test_file, (u8*)tmp_buf, TOP_FB_SZ+SUB_FB_SZ, &read_delay);
        f_close(&test_file);

        f_open(&test_file, test_fname, FA_READ);
    }

    REG_TM0VAL = 0; // Reset the timer
    REG_TM0CNT = 0x87; // Configure the timer to count up

    f_read(&test_file, TOP_SCREEN0, TOP_FB_SZ, &br_top);
    f_read(&test_file, BOT_SCREEN0, SUB_FB_SZ, &br_sub);

    REG_TM0CNT = 0x07; // Stop the timer
    read_delay = REG_TM0VAL; // Read the timers

    REG_TM0VAL = 0; // Reset the timer to 0

    f_close(&test_file);

    if ((br_top + br_sub) < (TOP_FB_SZ + SUB_FB_SZ)) // Amount of read data is less than intended. Sorry buddy, invalid test
        return 0; // GOTTA GO FAST

    return (u32)read_delay;
}

// Helper function, gets a random number from 0 to (max-1) and sets it up for loading
void load_animation(u32 max)
{
    u32 rngesus = REG_PRNG % max; // Dice roll

    // Load from /anim/'<rngesus>'/{anim, bottom_anim, config}
    char *cfg_fname  = "/anim/0/config.txt";
    char *top_fname  = "/anim/0/anim";
    char *sub_fname  = "/anim/0/bottom_anim";

    cfg_fname[6] = rngesus + '0'; // Set the number obtained + '0' in the string
    top_fname[6] = rngesus + '0'; // So it'll load "/anim/x/..." instead of "/anim/0/..."
    sub_fname[6] = rngesus + '0';

    FIL config_fil;
    size_t br = 0;
    if (f_open(&config_fil, cfg_fname, FA_READ) == FR_OK) // Open cfg_name
    {
        char cfg_buf[0x40] = {0};
        f_read(&config_fil, (void*)cfg_buf, 0x40, &br);

        u8 i;

        for (i = 0; (i < 2) && (i < br) && (cfg_buf[i] >= '0') && (cfg_buf[i] <= '9'); i++)
        {
            cfg[0] = (cfg[0] * 10) + (cfg_buf[i] - '0');
        }

        i++;

        if (!strcmp("lzd", &cfg_buf[i]))
            cfg[1] = 1;
    }

    if (cfg[0] < 1)
        cfg[0] = 15; // Let's avoid division by zero

    clear_screen(TOP_SCREEN0, 0x00);
    clear_screen(BOT_SCREEN0, 0x00);

    animation_loop(top_fname, sub_fname, cfg[0], cfg[1]); // Animation loop, with top_fname, sub_fname, cfg[0] framerate and cfg[1] compression (0 or 1 only)

    clear_screen(TOP_SCREEN0, 0x00);
    clear_screen(BOT_SCREEN0, 0x00);

    return;
}

void animation_loop(char *top_anim, char *bottom_anim, const u8 frame_rate, const u8 compression)
{
    u32 delay_ = (65457 / frame_rate), // 65457 = 1 sec, because the prescaler is set to 1024
        read_delay = get_read_delay(); // File read delay, detailed above

    size_t put_bot = 0, put_top = 0; // Read size vars
    FIL bgr_anim_bot, bgr_anim_top; // File handles

    char top_anim_flag = 0, // if 1 -> top animation exists
         sub_anim_flag = 0; // ^ same here, but with sub animation

    if (f_open(&bgr_anim_top, top_anim, FA_READ) == FR_OK) // If top animation opened fine
        top_anim_flag = 1; // Set top animation flag to 1

    if (f_open(&bgr_anim_bot, bottom_anim, FA_READ) == FR_OK) // If bottom animation opened fine
        sub_anim_flag = 1; // Set bottom animation flag to 1

    if (delay_ < read_delay) // Slow SDs
        delay_ = 0; // No delay whatsoever

    else
        delay_ -= read_delay; // Substract the read speed to the delay defined by the frame_rate

    size_t delay_top_only = ((delay_ * 240) / 400), // Delay to use when only the top anim is being played back
           delay_sub_only = ((delay_ * 240) / 320); // Delay to use when only the bottom anim is being played back
    // Numbers come from screen sizes

    switch(compression)
    {
        case 0: // RAW
        {
            while(1)
            {
                if (HID_PAD & (KEY_SELECT | KEY_START)) // End the animation if the 'SELECT' or 'START' key is pressed
                    break;

                if (top_anim_flag)
                {
                    f_read(&bgr_anim_top, TOP_SCREEN0, TOP_FB_SZ, &put_top); // Read an entire frame to framebuffer
                    if (put_top < TOP_FB_SZ) // If it couldn't read the entire frame...
                    {
                        top_anim_flag = 0; // Disable top animation, we reached EOF
                    }
                }  

                if (sub_anim_flag)
                {
                    f_read(&bgr_anim_bot, BOT_SCREEN0, SUB_FB_SZ, &put_bot); // ^ same as above
                    if (put_bot < SUB_FB_SZ)
                    {
                        sub_anim_flag = 0;
                    }
                }

                if (put_top != TOP_FB_SZ && put_bot != SUB_FB_SZ) // If none of the animations are playing, 
                    break;

                if (top_anim_flag && sub_anim_flag) // If both animations are playing, full delay
                    delay(delay_);

                else if (top_anim_flag) // If only top animation is playing, top_anim_delay
                    delay(delay_top_only);

                else if (sub_anim_flag) // ^ ditto
                    delay(delay_sub_only);
            }
            break;
        }

        case 1: // Compression method 1 (ban9comp, QuickLZ + delta decoding, by d0k3)
        {
            char top_frame_prev[TOP_FB_SZ] = {0}, top_frame_curr[TOP_FB_SZ] = {0}, top_frame_comp[TOP_FB_SZ + (TOP_FB_SZ/10) + 9] = {0}, // Set up buffers
                 sub_frame_prev[SUB_FB_SZ] = {0}, sub_frame_curr[SUB_FB_SZ] = {0}, sub_frame_comp[SUB_FB_SZ + (SUB_FB_SZ/10) + 9] = {0}; // For both animations, and compressed, uncompressed and delta
            // https://sourceforge.net/p/sevenzip/discussion/45797/thread/b6bd62f8/#4a3d
            // Given TOP_FB_SZ/SUB_FB_SZ are (relatively) small, it makes no sense to use (+ 64K)

            qlz_state_decompress *state_decompress = (qlz_state_decompress*)0x27F00000; // Hopefully there's nothing too interesting here :^)
            memset(state_decompress, 0, sizeof(qlz_state_compress)); // Clear...

            size_t comp_size = 0;

            while(1)
            {
                if (HID_PAD & (KEY_SELECT | KEY_START)) // End the animation if the 'SELECT' or 'START' key is pressed
                    break;

                if (top_anim_flag) // If there's a top animation...
                {
                    f_read(&bgr_anim_top, (void*)top_frame_comp, 9, &put_top); // Read top animations QuickLZ header
                    if (put_top != 9) // If it couldn't read the header
                    {
                        top_anim_flag = 0; // Disable the animation
                        goto skip_1;
                    }

                    comp_size = qlz_size_compressed(top_frame_comp); // Get compressed size

                    f_read(&bgr_anim_top, (void*)top_frame_comp + 9, comp_size - 9, &put_top); // Read compressed buffer

                    if (qlz_size_decompressed(top_frame_comp) != TOP_FB_SZ || put_top != (comp_size - 9)) // Check if EOF
                        break;

                    qlz_decompress(top_frame_comp, top_frame_curr, state_decompress); // Decompress the frame

                    for (size_t i = 0; i < TOP_FB_SZ; i++) // Perform delta decoding
                        top_frame_curr[i] += top_frame_prev[i];

                    memcpy(top_frame_prev, top_frame_curr, TOP_FB_SZ); // Keep a copy of the fb
                    memcpy(TOP_SCREEN0, top_frame_curr, TOP_FB_SZ); // Display it on the screen
                }

                if (sub_anim_flag) // If there's a bottom animation...
                {
                    f_read(&bgr_anim_bot, (void*)sub_frame_comp, 9, &put_bot); // same as above, but for bottom animation :)
                    if (put_bot != 9)
                    {
                        sub_anim_flag = 0;
                        goto skip_1;
                    }

                    comp_size = qlz_size_compressed(sub_frame_comp);

                    f_read(&bgr_anim_bot, (void*)sub_frame_comp + 9, comp_size - 9, &put_bot);

                    if (qlz_size_decompressed(sub_frame_comp) != SUB_FB_SZ || put_bot != (comp_size - 9))
                        break;

                    qlz_decompress(sub_frame_comp, sub_frame_curr, state_decompress);

                    for (size_t i = 0; i < SUB_FB_SZ; i++)
                        sub_frame_curr[i] += sub_frame_prev[i];

                    memcpy(sub_frame_prev, sub_frame_curr, SUB_FB_SZ);
                    memcpy(BOT_SCREEN0, sub_frame_curr, SUB_FB_SZ);
                }

                skip_1:

                if (top_anim_flag != 1 && sub_anim_flag != 1)
                    break;

                if (top_anim_flag && sub_anim_flag) // same as above as well
                    delay(delay_);

                else if (top_anim_flag)
                    delay(delay_top_only);

                else if (sub_anim_flag)
                    delay(delay_sub_only);
            }
            break;
        }

        if (top_anim_flag) // If top animation was opened, close it
            f_close(&bgr_anim_top);

        if (sub_anim_flag) // If bottom animation was opened, close it
            f_close(&bgr_anim_bot);
    }

    return;
}
