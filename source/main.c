#include <common.h>

#include <arm/asm.h>
#include <arm/ipc.h>
#include <disk/sdops.h>
#include <gfx/gfx.h>
#include <io/printf.h>
#include <io/term.h>
#include <zip/zip.h>

static console term;
static FATFS fs_main;

// NOTE: `chainload` uses an unprotected bootrom function - make sure it's accessible
void load_exec()
{
    void *payload = (void*)(0x23000000);
    u32 rb = file_read(payload, "0:/cakes/cakes.bin", MAX_PAYLOAD_SIZE);
    screen_init(GL_RGB8_OES); // Most payloads expect RGB24
    chainload(payload, rb);
}

void bax_main(void)
{
    init11();
    screen_init(GL_RGB565_OES);

    term_init(&term, ~0, TOP_SCREEN);
    // In case we have to output some data

    u32 mnt_ret = f_mount(&fs_main, "0:", 1),
        bt_env  = get_bootenv();

    /*
    Remember to enable on release!
    if (bt_env)
    { // Not a coldboot, chainload ASAP
        if (mnt_ret || !file_exists(ARM9_PAYLOAD_PATH)) power_off();
        else load_exec();
    }
    */

    printf("bt_env = %08X\n", bt_env);
    printf("mnt_ret = %08X\n\n", mnt_ret);

    char out[MAX_ANIMATIONS][_MAX_LFN + 1];
    u32 found = find_files(BASE_PATH, "*.zip", MAX_ANIMATIONS, out);
    // Search for all ZIP containers

    u32 dice = ctr_rand(), sel = (dice % found);

    printf("Found %d animation(s), PRNG returned %08X\n"
           "Would use animation %d\n\n", found, dice, sel);

    printf("BREAK!\n");

    __asm("bkpt\n\t");
}
