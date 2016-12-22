#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <common.h>

#include <arm/asm.h>

#include <disk/sdmmc/sdmmc.h>
#include <disk/file.h>

#include <gfx/gfx.h>

#include <io/printf.h>
#include <io/term.h>
#include <io/rand.h>

#include <zip/zip.h>

static console term;

// NOTE: `chainload` uses an unprotected bootrom function - make sure it's accessible
void load_exec()
{
    size_t rb = file_read((uint8_t*)UNUSED_MEM_LOC, ARM9_PAYLOAD_PATH, MAX_PAYLOAD_SIZE);
    chainload((uint8_t*)UNUSED_MEM_LOC, rb);
}

void main(void)
{
    uint32_t fs_ret = mount_fs(),
             bt_env = get_bootenv();

    if (bt_env)
    { // Not a coldboot, chainload inmediately
        if (fs_ret || !file_exists(ARM9_PAYLOAD_PATH)) ; // TODO: power_off();
        else load_exec();
    }

	term_init(&term, ~0, 0, TOP_SCREEN);
    // In case we have to output some data

    printf("mount_fs: %d\n", fs_ret);

    char out[MAX_ANIMATIONS][_MAX_LFN + 1];
    int found = find_files(BASE_PATH, "*.zip", MAX_ANIMATIONS, out);
    // Search for all ZIP containers

    uint32_t dice = ctrand();

    printf("Found %d animation(s), PRNG returned %08X\nWould use animation %d\n\n", found, dice, dice % found);

    for (int i = 0; i < found; i++)
    {
        printf("out[%d] = \"%s\", %d bytes\n", i, out[i], file_size(out[i]));

        uint8_t *zipfile = (uint8_t*)0x26000000;
        file_read(zipfile, out[i], 16 << 20);

        size_t sz;

        sz = zip_extract_file("top.ivf", (uint8_t*)0x27000000, zipfile, 16 << 20);
        if (sz) printf("Extracted top.ivf (%d bytes)\n", sz);
        else printf("Failed to extract top.ivf!\n");

        sz = zip_extract_file("bottom.ivf", (uint8_t*)0x27000000, zipfile, 16 << 20);
        if (sz) printf("Extracted bottom.ivf (%d bytes)\n", sz);
        else printf("Failed to extract bottom.ivf!\n");
    }

    printf("Done!\n");

    printf("Testing exception handler...\n");

    __asm("bkpt\n\t");

    while(1);
}
