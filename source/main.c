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

    if (!bax_abort()) {
        bax_start();
    }

    /* chainload */

    abort();
}
