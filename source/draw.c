#include "common.h"

char cfg[2] = {0x00}; // Current animations configuration
// cfg[0] is framerate, cfg[1] is the compression flag

const size_t TOP_FB_SZ = 288000; // 400 * 240 * 3
const size_t SUB_FB_SZ = 230400; // 320 * 240 * 3

framebuffer_t *framebuffer = (framebuffer_t *)0x23FFFE00; // Set framebuffers

// Given a framebuffers pointer, get its length
size_t fb_sz(u8 *fb)
{
    if (fb == framebuffer->bottom)
        return SUB_FB_SZ;

    else if (fb == framebuffer->top_left || fb == framebuffer->top_right)
        return TOP_FB_SZ;

    else
        return 0; // This should never happen unless I'm an idiot or someone messes with the source/binary (or both)
}

// Given a framebuffer and an rgb value, clear the screen
void clear_screen(u8 *fb, u32 rgb)
{
    size_t fb_len = fb_sz(fb), i = 0;
    while (i < fb_len)
    {
        *(u8*)(fb + i++) = rgb & 0xFF; // B value
        *(u8*)(fb + i++) = (rgb >> 8) & 0xFF; // G value (rgb shifted by one byte)
        *(u8*)(fb + i++) = (rgb >> 16) & 0xFF; // R value (rgb shifted by two bytes)
    }

    return;
}

// Read a file and get how much time it takes to read it
size_t get_read_delay()
{
    FIL test_file;
    FRESULT f_ret;
    size_t br_top, br_sub, read_delay;
    const char *test_fname = "/anim/calibrator"; // Hardcoded, maybe I'll do something else in the future

    f_ret = f_open(&test_file, test_fname, FA_READ);
    if (f_ret != FR_OK)
        return 0; // Run the thing as fast as possible

    REG_TM0CNT = 0b10000111; // Configure the timer to count up
    REG_TM0VAL = 0; // Reset the timer

    f_read(&test_file, framebuffer->top_left, TOP_FB_SZ, &br_top);
    f_read(&test_file, framebuffer->bottom, SUB_FB_SZ, &br_sub);

    read_delay = REG_TM0VAL; // Read the timers
    REG_TM0CNT = 0b00000111; // Stop the timer
    REG_TM0VAL = 0; // Reset the timer to 0

    f_close(&test_file);

    if ((br_top + br_sub) < (TOP_FB_SZ + SUB_FB_SZ)) // Amount of read data is less than intended. Sorry buddy, invalid test
        return 0; // GOTTA GO FAST

    // f_open and f_close are outside of the counter because they take additional time
    // Minimal error, but hey, if you have a precise timer don't let it go to waste :D

    return read_delay;
}

// Helper function, gets a random number from 0 to (max-1) and sets it up for loading
void load_animation(int max)
{
    int rngesus = REG_PRNG % max; // Dice roll

    // Load from /anim/'<rngesus>'/{anim, bottom_anim, config}
    char *cfg_fname  = "/anim/0/config";
    char *top_fname  = "/anim/0/anim";
    char *sub_fname  = "/anim/0/bottom_anim";

    memset(&cfg_fname[6], rngesus + '0', 1); // Set the number obtained + '0' in the string
    memset(&top_fname[6], rngesus + '0', 1); // So it'll load "/anim/x/..." instead of "/anim/0/..."
    memset(&sub_fname[6], rngesus + '0', 1);

    FIL config_fil;
    size_t br = 0;
    if (f_open(&config_fil, cfg_fname, FA_READ) == FR_OK) // Open cfg_name
    {
        f_read(&config_fil, &cfg, 2, &br); // Read the two configuration bytes
        f_close(&config_fil);
    }

    if (cfg[0] < 1)
        cfg[0] = 15; // Let's avoid division by zero/negatives, shall we?

    clear_screen(framebuffer->top_left, 0x00);
    clear_screen(framebuffer->bottom, 0x00);

    animation_loop(top_fname, sub_fname, cfg[0], cfg[1]); // Animation loop, with top_fname, sub_fname, cfg[0] framerate and cfg[1] compression (0 or 1 only)

    clear_screen(framebuffer->top_left, 0x00);
    clear_screen(framebuffer->bottom, 0x00);

    return;
}

// Thanks to hartmannaf for letting me know about 3dbrew.org/wiki/TIMER_Registers
inline void delay(size_t n)
{
    if (n == 0) // In case delay is 0
        return;

    REG_TM0VAL = 0; // Reset the timer
    REG_TM0CNT = 0b10000111; // Start timer count

    while(REG_TM0VAL < n); // Wait until I reach the desired tick

    REG_TM0CNT = 0b00000111; // Stop timer count
    REG_TM0VAL = 0; // Reset the timer
}

void animation_loop(char *top_anim, char *bottom_anim, const char frame_rate, const char compressed)
{
    size_t delay_ = (65457 / frame_rate), // 65457 = 1 sec, because the prescaler is set to 1024
           read_delay = get_read_delay(), // File read delay, detailed above
           put_bot = 0, put_top = 0; // Read size vars

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

    switch(compressed)
    {
        case 0: // Not compressed
        {
            while(1)
            {
                if (HID_PAD & (KEY_SELECT | KEY_START)) // End the animation if the 'SELECT' or 'START' key is pressed
                    break;

                if (top_anim_flag)
                {
                    f_read(&bgr_anim_top, framebuffer->top_left, TOP_FB_SZ, &put_top); // Read an entire frame to framebuffer
                    if (put_top < TOP_FB_SZ) // If it couldn't read the entire frame...
                    {
                        top_anim_flag = 0; // Disable top animation, we reached EOF
                    }
                }  

                if (sub_anim_flag)
                {
                    f_read(&bgr_anim_bot, framebuffer->bottom, SUB_FB_SZ, &put_bot); // ^ same as above
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

        case 1: // Compression method 1 (QuickLZ + delta decoding, by d0k3)
        {
            char top_frame_prev[TOP_FB_SZ], top_frame_curr[TOP_FB_SZ], top_frame_comp[TOP_FB_SZ + (TOP_FB_SZ/10) + 9], // Set up buffers
                 sub_frame_prev[SUB_FB_SZ], sub_frame_curr[SUB_FB_SZ], sub_frame_comp[SUB_FB_SZ + (SUB_FB_SZ/10) + 9]; // For both animations, and compressed, uncompressed and delta
            // https://sourceforge.net/p/sevenzip/discussion/45797/thread/b6bd62f8/#4a3d
            // Given TOP_FB_SZ/SUB_FB_SZ are (relatively) small, it makes no sense to use (+ 64K)

            // Clear buffers
            memset(top_frame_prev, 0, TOP_FB_SZ);
            memset(top_frame_curr, 0, TOP_FB_SZ);
            memset(top_frame_comp, 0, TOP_FB_SZ + (TOP_FB_SZ/10) + 9);
            memset(sub_frame_prev, 0, SUB_FB_SZ);
            memset(sub_frame_curr, 0, SUB_FB_SZ);
            memset(sub_frame_comp, 0, SUB_FB_SZ + (SUB_FB_SZ/10) + 9);

            qlz_state_decompress *state_decompress = (qlz_state_decompress*)0x25F00000; // Hopefully there's nothing too interesting here :^)
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
                    memcpy(framebuffer->top_left, top_frame_curr, TOP_FB_SZ); // Display it on the screen
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
                    memcpy(framebuffer->bottom, sub_frame_curr, SUB_FB_SZ);
                }

                skip_1: // Wynaut?

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
