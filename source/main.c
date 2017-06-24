#include <common.h>

#include <gfx/text.h>
#include <anim/bax.h>
#include <hw/timer.h>
#include <fs/ff/ff.h>

#include <gfx/gfx.h>

static FATFS DTCM  fs_main;
volatile bool DTCM *playing;

int main(UNUSED int argc, UNUSED char *argv[])
{
    clear_screen(GFX_TOP, ~0);

    if (f_mount(&fs_main, "", 1) != FR_OK) {
        abort();
    }

    clear_screen(GFX_BOTTOM, ~0);

    playing = bax_start();

    /* load payload into memory */

    /* loop until it stops playing */
    while(*playing);

    /* chainload */
    abort();

    while(1);
}
