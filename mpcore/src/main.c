#include <common.h>
#include <cache.h>
#include <cpu.h>
#include <sys.h>
#include <vram.h>
#include <interrupt.h>

#define PXI_CODE
#include <pxi.h>

#define PXICMD_CODE
#include <pxicmd.h>

#include "arm/irq.h"
#include "anim.h"
#include "hw/gx.h"
#include "hw/timer.h"

#include "arm/bugcheck.h"

#include "lib/ff/ff.h"

#define BAX_PATH "sdmc:/bax"
#define BAX_FIRM "/boot.firm"
#define BAX_FILE "*.bax"

#define FIRMSTUB_LOC (0x1FFFFC00)
extern u32 firmstub, firmstub_len;
int firmlaunch(void *firm, size_t firm_sz, const char *path)
{
    int res;
    void (*firmstub_reloc)(void) = (void (*)(void))FIRMSTUB_LOC;

    res = pxicmd_send(PXICMD_ARM9_FIRMVERIFY,
        (u32[]){(u32)firm, firm_sz}, 2);
    if (res != 0) return res;
    // ^ invalid FIRM

    // good FIRM, begin the boot procedure
    gx_reset();
    gx_set_framebuffer_mode(GL_RGB8);

    // Clear the entrypoint
    MPCORE_ENTRY = 0;

    // Relocate the boot stub
    memcpy((void*)FIRMSTUB_LOC, &firmstub, firmstub_len);
    _writeback_DC();
    _invalidate_IC();

    pxicmd_send(PXICMD_ARM9_FIRMBOOT,
        (u32[]){(u32)firm, (u32)path}, 2);
    /*
     There's a bit of a race here because the ARM9 could
     potentially overwrite AXIRAM before the MPCore gets
     to run the relocated firmstub.
    */

    firmstub_reloc();
    while(1) _wfi();
}

void pxi_handler(u32 irqn)
{
    u32 pxia[PXICMD_MAX_ARGC], pxic;
    u8 cmd = pxicmd_recv(pxia, &pxic);
    int resp = 0;

    cmd = pxicmd_recv(pxia, &pxic);
    switch(cmd)
    {
        default:
            break;
    }

    pxicmd_reply(resp);
    return;
}

void main(void)
{
    FATFS fs;
    int res;
    FIL fil;
    size_t sz, br;
    void *data;
    char bax_path[] = "sdmc:/bax/gba.bax";

    irq_register(IRQ_PXI_SYNC, pxi_handler, 0);
    pxi_reset();

    res = f_mount(&fs, "sdmc:", 1);
    if (res != FR_OK)
        bugcheck("FS_MOUNT", (u32[]){res}, 1);

    res = f_open(&fil, bax_path, FA_READ);

    if (res == FR_OK)
    {
        sz = f_size(&fil);
        if (sz > ANIM_MAX_SIZE)
            bugcheck("ANIM_BIG_SIZE", (u32[]){sz}, 1);

        data = malloc(sz);
        if (data == NULL)
            bugcheck("ANIM_ALLOC_ERR", NULL, 0);

        res = f_read(&fil, data, sz, &br);
        if (res != FR_OK || sz != br)
            bugcheck("FS_READ_ERROR", (u32[]){br}, 1);

        f_close(&fil);

        res = anim_validate((anim_t*)data, sz);
        if (res != ANIM_OK)
            bugcheck("ANIM_INVALID", (u32[]){res}, 1);

        anim_play((anim_t*)data);
        free(data);
    }

    while(1) _wfi();
}
