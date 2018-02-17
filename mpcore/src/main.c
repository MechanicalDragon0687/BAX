#include <common.h>
#include <cpu.h>
#include <lock.h>
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
            BUG(BUGSTR("OLDARM_PANIC"), 1, NULL, 0);

        default:
            break;
    }

    PXICMD_Reply(resp);
    return;
}

void load_boot_firm(void)
{
    int firm_r;
    void *firm;
    size_t firm_s;
    FS_File *firm_f;

    firm_f = FS_FileOpen(BAX_FIRM);
    firm_s = FS_FileSize(firm_f);
    if (firm_s > MAX_FIRM_SIZE)
        BUG(BUGSTR("FIRM_TOO_LARGE"), 1, BUGINT(firm_s), 1);

    firm = LockMalloc(firm_s);
    if (firm == NULL)
        BUG(BUGSTR("FIRM_ALLOC"), 1, BUGINT(firm_s), 1);

    FS_FileRead(firm_f, firm, firm_s);
    FS_FileClose(firm_f);

    firm_r = FIRM_Launch(firm, firm_s, BAX_FIRM);

    LockFree(firm);

    BUG(BUGSTR("FIRM_ERR"), 1, BUGINT((u32)firm, firm_s, firm_r), 3);
    while(1) CPU_WFI();
}

void main(void)
{
    FS_File *bax_f;
    FS_Dir *bax_d;
    size_t bax_s;
    u32 randseed, bootenv;

    bootenv  = PXICMD_Send(PXICMD_ARM9_BOOTENV, NULL, 0);
    randseed = PXICMD_Send(PXICMD_ARM9_RANDOM, NULL, 0);

    FS_Init();

    if (bootenv != 0)
        load_boot_firm();

    HID_Scan();
    if ((HID_Down() & HID_SKIP) == HID_SKIP)
        load_boot_firm();

    bax_d = FS_DirOpen(BAX_PATH);
    FS_DirSearch(bax_d, BAX_FILE, MAX_ANIMATIONS);

    if (FS_DirSearchCount(bax_d) > 0) {
        char bax_p[FS_MAXPATH];

        strcpy(bax_p, BAX_PATH "/");
        strcat(bax_p, FS_DirSearchResult(bax_d, randseed % FS_DirSearchCount(bax_d)));

        bax_f = FS_FileOpen(bax_p);
        bax_s = FS_FileSize(bax_f);

        if (bax_s > ANIM_MAX_SIZE)
            BUG(BUGSTR("ANIM_TOO_LARGE", bax_p), 2, BUGINT(bax_s), 1);

        BAX_Play(bax_f);

        FS_FileClose(bax_f);
    }

    FS_DirClose(bax_d);

    do {
        HID_Scan();
    } while(HID_Down() & HID_SKIP);

    load_boot_firm();
    while(1) CPU_WFI();
}
