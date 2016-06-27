#include "common.h"

u8 cfg[2] = {0}; // per-animation configuration
// cfg[0] is the "framerate", cfg[1] is the compression flag

u32 get_read_delay()
{
    FIL test_file;
    FRESULT f_ret;
    size_t br_top, br_sub, read_delay;

    f_ret = f_open(&test_file, CALIB_PATH, FA_READ);
    if (f_ret != FR_OK)
        return 0; // GOTTA GO FAST

    u8 tmp[TOP_FB_SZ] = {0};

    REG_TM0VAL = 0; // Reset the timer
    REG_TM0CNT = 0x87; // Configure the timer to count up

    // Read speed
    f_read(&test_file, TOP_SCREEN0, TOP_FB_SZ, &br_top);
    f_read(&test_file, BOT_SCREEN0, SUB_FB_SZ, &br_sub);

    // memcpy speed
    memcpy(tmp, TOP_SCREEN0, TOP_FB_SZ);
    memcpy(tmp, BOT_SCREEN0, SUB_FB_SZ);

    REG_TM0CNT = 0x07; // Stop the timer
    read_delay = REG_TM0VAL; // Read the timers

    REG_TM0VAL = 0; // Reset the timer to 0

    f_close(&test_file);

    if ((br_top + br_sub) < (TOP_FB_SZ + SUB_FB_SZ)) // Amount of read data is less than intended, sort of invalid test?
        return 0;

    return (u32)read_delay;
}

// Helper function, gets a random number from 0 to (max-1) and loads it
void load_animation(u32 max)
{
    u32 rng = (REG_PRNG % max); // Dice roll

    // Load /anim/'<rng>'/{anim, bottom_anim, config.txt}
    char cfg_fname[] = CFG_ANIM_PATH, // "/anim/0/config.txt"
         top_fname[] = TOP_ANIM_PATH, // "/anim/0/anim"
         sub_fname[] = SUB_ANIM_PATH; // "/anim/0/bottom_anim"

    cfg_fname[6] = rng + '0'; // Set the number obtained + '0' in the string
    top_fname[6] = rng + '0'; // So it'll load "/anim/x/..." instead of "/anim/0/..."
    sub_fname[6] = rng + '0';

    FIL cfg_fil;
    size_t br = 0;
    if (f_open(&cfg_fil, cfg_fname, FA_READ) == FR_OK) // Open cfg_name
    {
        char cfg_buf[0x10] = {0};

        f_read(&cfg_fil, (void*)cfg_buf, 0x10, &br); // Read up to 16 bytes from the config file
        f_close(&cfg_fil);

        if (!br) // File size is 0, skip
            goto skip;

        u8 i;

        for (i = 0; (i < 2) && (i < br) && (cfg_buf[i] >= '0') && (cfg_buf[i] <= '9'); i++)
        {
            // Read up to 2 chars and convert them to regular ints, making sure they're within '0' and '9'
            cfg[0] = (cfg[0] * 10) + (cfg_buf[i] - '0');
        }

        i++;

        if (!strcmp("lzd", &cfg_buf[i]))
            cfg[1] = 1; // If string is "lzd", then it uses ban9comp
    }

    skip:

    if (cfg[0] < 1)
        cfg[0] = 15; // Let's avoid division by zero

    animation_loop(top_fname, sub_fname, cfg[0], cfg[1]);
    // Animation loop, cfg[0] is framerate and cfg[1] is compression (0 or 1 currently)

    clear_screen(TOP_SCREEN0, 0);
    clear_screen(BOT_SCREEN0, 0);

    return;
}

void animation_loop(char *top_anim, char *bottom_anim, const u8 fps, const u8 compression)
{
    u32 delay_base = (65457 / fps), // 65457 = 1 sec, because the prescaler is set to 1024
        read_delay = get_read_delay(); // File read delay, detailed above

    FIL bgr_anim_top,
        bgr_anim_bot;  // File handles

    size_t put_top = 0,
           put_bot = 0; // Read size vars

    u8 top_anim_flag = 0, top_ = 0,  // if 1 -> top animation exists
       sub_anim_flag = 0, sub_ = 0;  // ^ same here, but with bottom animation
       
    char top_frame_prev[TOP_FB_SZ] = {0}, sub_frame_prev[SUB_FB_SZ] = {0};

    if (f_open(&bgr_anim_top, top_anim, FA_READ) == FR_OK) // If top animation opened fine
    {
        top_anim_flag = 1; // Set top animation flag to 1
        top_ = 1;
    }

    if (f_open(&bgr_anim_bot, bottom_anim, FA_READ) == FR_OK) // If bottom animation opened fine
    {
        sub_anim_flag = 1; // Set bottom animation flag to 1
        sub_ = 1;
    }

    if (delay_base < read_delay) // Slow SDs
        delay_base = 0; // No delay whatsoever

    else
        delay_base -= read_delay; // Substract the read speed to the delay defined by the fps

    u32 delay_top_only = ((delay_base * 240) / 400), // Delay to use when only the top anim is being played back
        delay_sub_only = ((delay_base * 240) / 320); // Delay to use when only the bottom anim is being played back
    // Numbers come from screen size

    switch(compression)
    {
        case 0: // RAW
        {
            while(1)
            {
                if (HID_PAD & (KEY_SELECT | KEY_START)) // End the animation if the 'SELECT' or 'START' key is pressed
                    break;

                if (top_)
                {
                    f_read(&bgr_anim_top, top_frame_prev, TOP_FB_SZ, &put_top); // Read an entire frame to framebuffer

                    if (put_top < TOP_FB_SZ) // If it couldn't read the entire frame...
                        top_ = 0; // Disable top animation, we reached EOF

                    memcpy(TOP_SCREEN0, top_frame_prev, TOP_FB_SZ);
                }  

                if (sub_)
                {
                    f_read(&bgr_anim_bot, sub_frame_prev, SUB_FB_SZ, &put_bot); // ^ same as above

                    if (put_bot < SUB_FB_SZ)
                        sub_ = 0;

                    memcpy(BOT_SCREEN0, sub_frame_prev, SUB_FB_SZ);
                }

                if (!top_ && !sub_)
                    break;

                if (top_ && sub_) // If both animations are playing, full delay
                    delay(delay_base);

                else
                    delay(top_ ? delay_top_only : delay_sub_only);
            }
            break;
        }

        case 1: // Compression method 1 (ban9comp, QuickLZ + delta decoding, by d0k3)
        {
            size_t comp_size = 0;
            char top_frame_curr[TOP_FB_SZ] = {0}, top_frame_comp[TOP_FB_SZ + (TOP_FB_SZ/10) + 9] = {0}, // Set up buffers
                 sub_frame_curr[SUB_FB_SZ] = {0}, sub_frame_comp[SUB_FB_SZ + (SUB_FB_SZ/10) + 9] = {0}; // For both animations, and compressed, uncompressed and delta
            // https://sourceforge.net/p/sevenzip/discussion/45797/thread/b6bd62f8/#4a3d
            // Given TOP_FB_SZ/SUB_FB_SZ are (relatively) small, it makes no sense to use (+ 64K)

            qlz_state_decompress *state_decompress = (qlz_state_decompress*)0x27F00000; // Hopefully there's nothing too interesting here :^)
            memset(state_decompress, 0, sizeof(qlz_state_decompress));

            while(1)
            {
                if (HID_PAD & (KEY_SELECT | KEY_START)) // End the animation if the 'SELECT' or 'START' key is pressed
                    break;

                if (top_) // If there's a top animation...
                {
                    f_read(&bgr_anim_top, (void*)top_frame_comp, 9, &put_top); // Read the top animations' QuickLZ header
                    if (put_top != 9) // If it couldn't read the header
                    {
                        top_ = 0; // Disable the animation
                        goto skip_1;
                    }

                    comp_size = qlz_size_compressed(top_frame_comp); // Get compressed size

                    f_read(&bgr_anim_top, (void*)top_frame_comp + 9, comp_size - 9, &put_top); // Read compressed buffer

                    if (qlz_size_decompressed(top_frame_comp) != TOP_FB_SZ || put_top != (comp_size - 9)) // Check if invalid frame data / EOF
                        break;

                    qlz_decompress(top_frame_comp, top_frame_curr, state_decompress); // Decompress the frame

                    for (u32 i = 0; i < TOP_FB_SZ; i++) // Perform delta decoding
                        top_frame_curr[i] += top_frame_prev[i];

                    memcpy(top_frame_prev, top_frame_curr, TOP_FB_SZ); // Keep a copy of the fb
                    memcpy(TOP_SCREEN0, top_frame_curr, TOP_FB_SZ); // Display it on the screen
                }

                if (sub_) // If there's a bottom animation...
                {
                    f_read(&bgr_anim_bot, (void*)sub_frame_comp, 9, &put_bot); // same as above, but for bottom animation :)
                    if (put_bot != 9)
                    {
                        sub_ = 0;
                        goto skip_1;
                    }

                    comp_size = qlz_size_compressed(sub_frame_comp);

                    f_read(&bgr_anim_bot, (void*)sub_frame_comp + 9, comp_size - 9, &put_bot);

                    if (qlz_size_decompressed(sub_frame_comp) != SUB_FB_SZ || put_bot != (comp_size - 9))
                        break;

                    qlz_decompress(sub_frame_comp, sub_frame_curr, state_decompress);

                    for (u32 i = 0; i < SUB_FB_SZ; i++)
                        sub_frame_curr[i] += sub_frame_prev[i];

                    memcpy(sub_frame_prev, sub_frame_curr, SUB_FB_SZ);
                    memcpy(BOT_SCREEN0, sub_frame_curr, SUB_FB_SZ);
                }

                skip_1:

                if (!top_ && !sub_)
                    break;

                if (top_ && sub_) // same as above as well
                    delay(delay_base);
                else
                    delay(top_ ? delay_top_only : delay_sub_only);
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
