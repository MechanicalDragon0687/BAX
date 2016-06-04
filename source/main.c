#include "common.h"
#include "loader.h" // loader binary converted to a char array, designed to load @ 0x24F00000

static FATFS fs;
const char main_payload[] = "/anim/arm9payload.bin";
framebuffer_t *framebuffer;

void chainload() // Load and execute the chainloader (hopefully)
{
    if (file_exists(main_payload)) // If arm9payload.bin or luma.bin exist...
    {
        memcpy((void*)0x24F00000, loader_bin, loader_bin_len); // Load the chainloader
    }

    else // u dun goof ;)
    {
        clear_screen(framebuffer->top_left, 0x00); // Clear screen
        draw_str(framebuffer->top_left, "No payload detected", 8, 8, 0xFFFFFF);
        draw_str(framebuffer->top_left, "Make sure you have /anim/arm9payload.bin", 8, 24, 0xFFFFFF);
        while(1);
    }

    ((void(*)(void))0x24F00000)(); // Jump to the loaded payload
}

int check_anims()
{
    char  *top    = "/anim/0/anim",
          *bottom = "/anim/0/bottom_anim";

    int retval = 0; // Declare value to return

    for (int i = 0; i < 10; i++) // Check files from dir '0' to '9'
    {
        memset(&top[6], i + '0', 1); // Set the '0' in the string to '0' + x, 0 <= x < 10
        memset(&bottom[6], i + '0', 1);

        if (file_exists(top) + file_exists(bottom)) // If at least one of them exists
            retval++; // Increase return value

        else
            break; // If neither exists break out
    }

    return retval; // Return retval, should be between 0 and 9
}

void main()
{
    if (f_mount(&fs, "0:", 1) != FR_OK) // Mount the SD card
        chainload(); // Try to chainload if mounting fails, shouldn't work but you never know ;)

    int amt = check_anims(); // Check amount of animations

    if (!(CFG_BOOTENV) & !(HID_PAD & KEY_LT) && (amt > 0)) // Check if this is a coldboot or R trigger is pressed, and make sure there's at least one animation
        load_animation(amt); // Load randomizer

    chainload(); // When it finishes , amt == 0 or left trigger is held, chainload

    return;
}
