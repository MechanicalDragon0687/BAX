#include <common.h>

#include <anim/bax.h>
#include <fs/ff/ff.h>

int main(UNUSED int argc, UNUSED char *argv[])
{
    FATFS fs_main;
    FRESULT res;

    res = f_mount(&fs_main, "", 1);
    if (res != FR_OK) {
        abort_code(res);
    }

    if (!abort_anim()) {
        bax_start();
    }

    /* load binary into memory */
    /* chainload */

    abort();
}
