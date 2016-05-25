#include "common.h"
#include "loader.h" // loader binary converted to a char array, designed to load @ 0x24F00000

static FATFS fs;
const char bctr9_payload[] = "/arm9bootloader.bin", main_payload[] = "/anim/arm9payload.bin", luma_payload[] = "/anim/luma.bin";
framebuffer_t *framebuffer;

void chainload() // Load and execute the chainloader
{
    if (!file_exists(main_payload) || !file_exists(luma_payload)) // Special case for BootCTR9
    {
        memcpy((void*)0x24F00000, loader_bin, loader_bin_len);
    }

    else if (file_exists(bctr9_payload))
    {
        FIL bctr9_fil;
        size_t bctr9_len;
        f_open(&bctr9_fil, bctr9_payload, FA_READ);
        f_read(&bctr9_fil, (void*)0x24F00000, 0x200000, &bctr9_len);
        f_close(&bctr9_fil);
    }

    else // u dun goof ;)
    {
        clear_screen(framebuffer->top_left, 0xFF);
        while(1);
    }

    ((void(*)(void))0x24F00000)();
}

int check_files()
{
    char  *top    = "/anim/0/anim",
          *bottom = "/anim/0/bottom_anim";

    int retval = 0;

    for (int i = 0; i < 10; i++) // Check files from dir '0' to '9'
    {
        memset(&top[6], i + '0', 1);
        memset(&bottom[6], i + '0', 1);

        if (file_exists(top) + file_exists(bottom)) // If at least one of them exists
            retval++;

        else
            break;
    }
    return retval;
}

void main()
{
    if (f_mount(&fs, "0:", 1) != FR_OK) // Mount the SD card
        chainload(); // Try to chainload if mounting fails, shouldn't work but you never know ;)

    int amt = check_files();

    if (!(CFG_BOOTENV) & !(HID_PAD & KEY_LT) && (amt > 0)) // Check if this is a coldboot or R trigger is pressed
        load_animation(amt); // Load animations

    chainload(); // When it finishes or amt == 0, chainload

    return;
}
