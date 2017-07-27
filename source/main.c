#include <common.h>

#include <anim/bax.h>
#include <gfx/gfx.h>
#include <fs/ff/ff.h>

int main(UNUSED int argc, UNUSED char *argv[], UNUSED char *envp[])
{
    FATFS fs_main;
    FRESULT res;

    clear_screen(GFX_SUB, GFX_GREEN);

    res = f_mount(&fs_main, "", 1);
    assert(res == FR_OK);

    clear_screen(GFX_MAIN, GFX_RED);

    if (bax_playable()) {
        bax_start();
    }

    /* chainload */
    abort();
}
