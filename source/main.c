#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <common.h>

#include <arm/asm.h>

#include <disk/sdops.h>

#include <gfx/gfx.h>

#include <io/printf.h>
#include <io/term.h>
#include <io/rand.h>

#include <zip/zip.h>

static console term;
static FATFS fs_main;

// NOTE: `chainload` uses an unprotected bootrom function - make sure it's accessible
void load_exec()
{
    size_t rb = file_read((uint8_t*)UNUSED_MEM_LOC, ARM9_PAYLOAD_PATH, MAX_PAYLOAD_SIZE);
    chainload((uint8_t*)UNUSED_MEM_LOC, rb);
}

void bax_main(void)
{
    term_init(&term, ~0, 0, TOP_SCREEN);
    // In case we have to output some data

    printf("Initialized console output\n");

    uint32_t mnt_ret = f_mount(&fs_main, "0:", 0),
             bt_env  = get_bootenv();

    printf("bt_env = %08X\n", bt_env);
    printf("mnt_ret = %08X\n\n", mnt_ret);


    if (bt_env)
    { // Not a coldboot, chainload ASAP
        if (mnt_ret || !file_exists(ARM9_PAYLOAD_PATH)) ; // TODO: power_off();
        else load_exec();
    }

    char out[MAX_ANIMATIONS][_MAX_LFN + 1];
    uint32_t found = find_files(BASE_PATH, "*.zip", MAX_ANIMATIONS, out);
    // Search for all ZIP containers

    uint32_t dice = ctrand(),
             sel  = dice % found;

    printf("Found %d animation(s), PRNG returned %08X\nWould use animation %d\n\n", found, dice, sel);

    for (uint32_t i = 0; i < found; i++)
    {
        char *fpath = out[i];
        uint8_t *zipfile = (uint8_t*)0x26000000, *zipdest = (uint8_t*)0x27000000;
        size_t sz;

        printf("out[%d] = \"%s\", ", i, fpath, file_size(fpath));
        sz = file_read(zipfile, out[i], 16 << 20);
        printf("read %d bytes\n", sz);

        printf("attempting to extract top.ivf ");
        sz = zip_extract_file("top.ivf", zipdest, zipfile, 16 << 20);
        printf("(%d bytes)\n", sz);

        printf("attempting to extract bottom.ivf ");
        sz = zip_extract_file("bottom.ivf", zipdest, zipfile, 16 << 20);
        printf("(%d bytes)\n", sz);

        printf("\n");
    }

    printf("Done!\n"
           "Testing exception handler...\n");

    __asm("bkpt\n\t");

    while(1);
}
