#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <common.h>
#include <arm/asm.h>
#include <arm/ipc.h>
#include <disk/sdops.h>
#include <gfx/gfx.h>
#include <io/printf.h>
#include <io/term.h>
#include <ivf/ivf.h>
#include <zip/zip.h>

static console term;
static FATFS fs_main;

// NOTE: `chainload` uses an unprotected bootrom function - make sure it's accessible
void load_exec()
{
    size_t rb = file_read((void*)0x23000000, "0:/cakes/cakes.bin", MAX_PAYLOAD_SIZE);
    screen_init(GL_RGB8_OES);
    chainload((void*)0x23000000, rb);
}

void bax_main(void)
{
    init11();
    screen_init(GL_RGB565_OES);

    term_init(&term, ~0, TOP_SCREEN);
    // In case we have to output some data

    uint32_t mnt_ret = f_mount(&fs_main, "0:", 0),
             bt_env  = get_bootenv();

    /*
    Remember to enable on release!
    if (bt_env)
    { // Not a coldboot, chainload ASAP
        if (mnt_ret || !file_exists(ARM9_PAYLOAD_PATH)) ; // TODO: power_off();
        else load_exec();
    }
    */

    printf("bt_env = %08X\n", bt_env);
    printf("mnt_ret = %08X\n\n", mnt_ret);

    char out[MAX_ANIMATIONS][_MAX_LFN + 1];
    uint32_t found = find_files(BASE_PATH, "*.zip", MAX_ANIMATIONS, out);
    // Search for all ZIP containers

    uint32_t dice = ctr_rand(), sel = (dice % found);

    printf("Found %d animation(s), PRNG returned %08X\n"
           "Would use animation %d\n\n", found, dice, sel);

    printf("Done!\n"
           "Testing exception handler...\n");

    //load_exec();

    __asm("bkpt\n\t");
}
