#include <common.h>
#include <cache.h>
#include <cpu.h>
#include <pxicmd.h>

#include "arm/bug.h"
#include "lib/fs/fs.h"
#include "hw/hid.h"
#include "anim.h"


// Arbitrary
#define MAX_ANIMATIONS (64)
#define MAX_FIRM_SIZE  (0x400000)

#define HID_SKIP       (HID_X)

int FIRM_Launch(void *firm, size_t firm_sz, const char *firm_path);


void PXICMD_Handler(u32 irqn)
{
    u32 pxia[PXICMD_MAX_ARGC];
    int resp = 0, pxic;
    u8 cmd = PXICMD_Recv(pxia, &pxic);

    switch(cmd) {
        case PXICMD_ARM11_PANIC:
            BUG(BUGSTR("OLDARM PANIC"), 1, pxia, pxic);

        default:
            BUG(BUGSTR("PXICMD HANDLER"), 1, BUGINT(cmd, pxic), 2);
            break;
    }

    PXICMD_Reply(resp);
    return;
}

void FIRM_LoadBoot(void)
{
    int firm_res;
    void *firm_dat;
    size_t firm_sz;
    FS_File *firm_fs;
    char bax_path[FS_MAXPATH] = {0};

    if (FS_FileExists(BAX_FIRMSRCP)) {
        firm_fs = FS_FileOpen(BAX_FIRMSRCP);
        firm_sz = FS_FileSize(firm_fs);

        if (firm_sz >= FS_MAXPATH)
            firm_sz = FS_MAXPATH - 1;

        FS_FileRead(firm_fs, bax_path, firm_sz);
        FS_FileClose(firm_fs);
    } else {
        strcpy(bax_path, BAX_FIRMFILE);
    }

    firm_fs = FS_FileOpen(bax_path);
    firm_sz = FS_FileSize(firm_fs);
    if (firm_sz > MAX_FIRM_SIZE)
        BUG(BUGSTR("FIRM TOO LARGE"), 1, BUGINT(firm_sz), 1);

    firm_dat = malloc(firm_sz);
    assert(firm_dat != NULL);

    FS_FileRead(firm_fs, firm_dat, firm_sz);
    FS_FileClose(firm_fs);

    CACHE_WbDCRange(bax_path, FS_MAXPATH);
    firm_res = FIRM_Launch(firm_dat, firm_sz, bax_path);

    free(firm_dat);

    BUG(BUGSTR("FIRM VALIDATE"), 1, BUGINT((u32)firm_dat, firm_sz, firm_res), 3);
    while(1) CPU_WFI();
}

void main(void)
{
    FS_File *bax_f;
    FS_Dir *bax_d;
    size_t bax_s;
    char *bax_p;
    u32 randseed, bootenv;

    bootenv  = PXICMD_Send(PXICMD_ARM9_BOOTENV, NULL, 0);
    randseed = PXICMD_Send(PXICMD_ARM9_RANDOM, NULL, 0);

    FS_Init();

    if (bootenv != 0)
        FIRM_LoadBoot();

    HID_Scan();
    if ((HID_Down() & HID_SKIP) == HID_SKIP)
        FIRM_LoadBoot();

    bax_d = FS_DirOpen(BAX_PATH);
    FS_DirSearch(bax_d, BAX_FILE, MAX_ANIMATIONS);

    if (FS_DirSearchCount(bax_d) > 0) {
        bax_p = FS_DirSearchResult(bax_d, randseed % FS_DirSearchCount(bax_d));
        bax_f = FS_FileOpen(bax_p);
        bax_s = FS_FileSize(bax_f);

        if (bax_s > ANIM_MAX_SIZE)
            BUG(BUGSTR("ANIM TOO LARGE", bax_p), 2, BUGINT(bax_s), 1);

        BAX_Play(bax_f, HID_SKIP);

        FS_FileClose(bax_f);
    }

    FS_DirClose(bax_d);

    do {
        HID_Scan();
    } while(HID_Down() & HID_SKIP);

    FIRM_LoadBoot();
    while(1) CPU_WFI();
}
