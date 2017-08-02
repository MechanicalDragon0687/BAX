#include <common.h>

#include <anim.h>
#include <fs/ff/ff.h>

int main(int argc, char *argv[])
{
    FATFS fs_main;
    FIL baxfil;
    FRESULT res;
    size_t bs, br;
    anim_header *hdr;

    /*
    res = f_mount(&fs_main, "", 1);

    if (res != FR_OK) {
        //poweroff();
    }

    res = f_open(&baxfil, BASE_PATH ANIM_SUBPATH, FA_READ);
    if (res == FR_OK) {
        //poweroff();
    }
    */
    while(1);
}
