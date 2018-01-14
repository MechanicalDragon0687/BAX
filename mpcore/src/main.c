#include <common.h>
#include <cache.h>
#include <pxi.h>
#include <cpu.h>
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
    u32 pxia[PXICMD_MAX_ARGC];
    int resp = 0;

    cmd = pxicmd_recv(pxia);
    switch(PXICMD_CMDID(cmd))
    {
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
    FIL bax_file;
    size_t bax_size, br;
    anim_t *bax_data;
    char bax_path[FF_MAX_LFN + 1] = {0};

    irq_register(IRQ_PXI_SYNC, pxi_handler);
    pxi_reset();

    res = f_mount(&fs, "sdmc:", 1);
    if (res != FR_OK)
        _bugcheck("bad_mount");

    find_path(BAX_PATH, BAX_FILE, bax_path);

    if (strlen(bax_path) == 0)
        _bugcheck("bad_path");

    res = f_open(&bax_file, bax_path, FA_READ);
    if (res != FR_OK)
        _bugcheck("bad_open");

    bax_size = f_size(&bax_file);
    if (bax_size > ANIM_MAX_SIZE)
        _bugcheck("bad_size");

    bax_data = malloc(bax_size);
    if (bax_data == NULL)
        _bugcheck("bad_data");

    res = f_read(&bax_file, (void*)bax_data, bax_size, &br);
    if (res != FR_OK || bax_size != br)
        _bugcheck("bad_read");

    res = anim_validate(bax_data, bax_size);
    if (res != ANIM_OK)
        _bugcheck("bad_anim");

    anim_play(bax_data);
    _bugcheck("done");
    while(1);
}
