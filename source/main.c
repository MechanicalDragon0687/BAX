#include "common.h"
#include "anim.h"
#include "draw.h"
#include "i2c.h"
#include "fatfs/ff.h"
#include "fatfs/sdmmc/sdmmc.h"

static FATFS fs;

void poweroff()
{
    i2cWriteRegister(0x03, 0x20, 1);
    while(1);
}

/** If there were any use for arguments, I'd bring them back */
int main()
{
    if (PDN_GPU_CNT == 1) // no screen init is not supported bruh
        poweroff();

    if (f_mount(&fs, "0:", 0) != FR_OK) // Mount the SD card
        error("Failed to mount the sd card");

    if (!CFG_BOOTENV) // Check if this is a coldboot and make sure there's at least one animation
        load_animation(); // Load randomizer

    chainload();

    return 0;
}
