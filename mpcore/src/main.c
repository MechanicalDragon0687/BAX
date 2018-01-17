#include <common.h>
#include <cpu.h>
#include <sys.h>

#define CACHE_CODE
#include <cache.h>

#define PXI_CODE
#include <pxi.h>

#define IRQ_CODE
#include <irq.h>

#define PXICMD_CODE
#include <pxicmd.h>

#include "anim.h"
#include "hw/int.h"
#include "hw/timer.h"

#include "arm/bugcheck.h"

#include "lib/ff/ff.h"

#define BAX_PATH "sdmc:/bax"
#define BAX_FIRM "/boot.firm"
#define BAX_FILE "*.bax"

void pxi_handler(u32 irqn)
{
    u8 cmd;
    u32 pxia[PXICMD_MAX_ARGC], pxic;
    int resp = 0;

    cmd = pxicmd_recv(pxia, &pxic);
    switch(cmd)
    {
        case PXICMD_ARM11_STUBOUT:
        {
            resp = ~0;
            break;
        }
        default:
        // TODO: bugcheck
            break;
    }

    pxicmd_reply(resp);
    return;
}

void find_path(const char *path, const char *pattern, char *out)
{
    FRESULT fres;
    DIR dir;
    FILINFO fno;
    int pos = 0;

    fres = f_findfirst(&dir, &fno, path, pattern);

    if (fres == FR_OK && fno.fname[0])
    {
        strcpy(out, path);
        out[strlen(out)] = '/';
        strcat(out, fno.fname);
    }
    else
        memset(out, 0, FF_MAX_LFN);

    f_closedir(&dir);
    return;
}

void main(void)
{
    FATFS fs;
    int res;
    FIL fil;
    size_t sz, br;
    void *data;
    char bax_path[FF_MAX_LFN + 1] = {0};

    irq_register(IRQ_PXI_SYNC, pxi_handler);
    pxi_reset();

    res = f_mount(&fs, "sdmc:", 1);
    if (res != FR_OK)
        _bugcheck("bad_mount");

    find_path(BAX_PATH, BAX_FILE, bax_path);

    if (strlen(bax_path) == 0)
        _bugcheck("bad_path");

    res = f_open(&fil, bax_path, FA_READ);
    if (res != FR_OK)
        _bugcheck("bad_open");

    sz = f_size(&fil);
    if (sz > ANIM_MAX_SIZE)
        _bugcheck("bad_size");

    data = malloc(sz);
    if (data == NULL)
        _bugcheck("bad_data");

    res = f_read(&fil, data, sz, &br);
    if (res != FR_OK || sz != br)
        _bugcheck("bad_read");

    f_close(&fil);

    res = anim_validate((anim_t*)data, sz);
    if (res != ANIM_OK)
        _bugcheck("bad_anim");

    anim_play((anim_t*)data);
    free(data);
    while(1) _wfi();
}
