#include "common.h"

static FATFS fs;

void poweroff()
{
    i2cWriteRegister(0x03, 0x20, 1);
    while(1);
}

u32 check_anims()
{
    char top[]    = TOP_ANIM_PATH,
         bottom[] = SUB_ANIM_PATH;

    u32 retval = 0; // Return value

    for (u32 i = 0; i < 10; i++) // Check files from dir '0' to '9'
    {
        top[6]    = i + '0'; // Set the '0' in the string to '0' + i, therefore 0 <= i < 10
        bottom[6] = i + '0';

        if (file_exists(top) || file_exists(bottom)) // If at least one of them exists
            retval++; // Increase return value

        else
            break; // If neither exists break out
    }

    return retval; // Return retval, should be between 0 and 9
}

/** If there were any use for arguments, I'd bring them back */
int main()
{
    if (PDN_GPU_CNT == 1) // no screen init is not supported bruh
        poweroff();

    if (f_mount(&fs, "0:", 1) != FR_OK) // Mount the SD card
        error("Failed to mount the sd card");

    u32 amt = check_anims(); // Check amount of animations

    if ((CFG_BOOTENV == 0) && (amt > 0)) // Check if this is a coldboot and make sure there's at least one animation
        load_animation(amt); // Load randomizer

    chainload();

    return 0;
}
