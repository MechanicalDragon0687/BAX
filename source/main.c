#include "common.h"
#include "loader.h" // loader binary converted to a char array, designed to load @ 0x24F00000

static FATFS fs;

void chainload() // Load and execute the chainloader
{
	u32 destination = 0x24F00000;
    memcpy((void*)destination, loader_bin, loader_bin_len);
    ((void(*)(void))destination)();
}

u32 check_files()
{
    char  *top    = "/anim/0/anim",
          *bottom = "/anim/0/bottom_anim";

    u32 count = 0;

    for (u32 i = 0; i < 10; i++) { // Check files from dir '0' to '9'
        top[6]    = i + '0';
        bottom[6] = i + '0';

        if (file_exists(top) + file_exists(bottom)) // If at least one of them exists
            count++;

        else
            break; // Otherwise, break the loop
    }
    return count;
}

void main()
{
    if (f_mount(&fs, "0:", 1) != FR_OK) // Mount the SD card
        chainload(); // Try to chainload if mounting fails, shouldn't work but you never know ;)

    u32 amt = check_files();

    if (!(CFG_BOOTENV) & !(HID_PAD & KEY_LT) && (amt < 10)) // Check if this is a coldboot or R trigger is pressed
        load_animation(amt); // Load animations

    chainload(); // When it finishes or amt == 0, chainload

    return;
}
