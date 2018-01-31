#include <common.h>
#include <pxicmd.h>

#include "arm/bugcheck.h"
#include "lib/fs/fs.h"
#include "hw/hid.h"
#include "anim.h"

#define BAX_PATH "sdmc:/bax"
#define BAX_FILE "*.bax"
#define BAX_FIRM BAX_PATH"/boot.firm"

// Arbitrary
#define MAX_ANIMATIONS (32)
#define MAX_FIRM_SIZE  (0x400000)

int firmlaunch(void *firm, size_t firm_sz, const char *firm_path);


void pxi_handler(u32 irqn)
{
    u32 pxia[PXICMD_MAX_ARGC], pxic;
    u8 cmd = pxicmd_recv(pxia, &pxic);
    int resp = 0;

    cmd = pxicmd_recv(pxia, &pxic);
    switch(cmd)
    {
        case PXICMD_ARM11_PANIC:
            bugcheck("OLDARM_PANIC", NULL, 0);

        default:
            break;
    }

    pxicmd_reply(resp);
    return;
}

void load_boot_firm(void)
{
    int firm_err;
    size_t firm_sz;
    void *firm;

    firm_sz = fs_size(BAX_FIRM);
    if (firm_sz == 0)
        bugcheck("FIRM_NOT_FOUND", NULL, 0);
    else if (firm_sz > MAX_FIRM_SIZE)
        bugcheck("FIRM_TOO_LARGE", (u32*)&firm_sz, 1);

    firm = malloc(firm_sz);
    if (firm == NULL)
        bugcheck("FIRM_MALLOC", (u32*)&firm_sz, 1);

    fs_read(BAX_FIRM, firm, firm_sz);
    firm_err = firmlaunch(firm, firm_sz, BAX_FIRM);

    bugcheck("FIRM_ERR", (u32[]){(u32)firm, firm_sz, firm_err}, 3);
    while(1) _wfi();
}

void main(void)
{
    int res, anim_count;
    u32 rnd, bootenv;
    char *anim_paths[MAX_ANIMATIONS];

    bootenv = pxicmd_send(PXICMD_ARM9_BOOTENV, NULL, 0);
    rnd = pxicmd_send(PXICMD_ARM9_RANDOM, NULL, 0);

    res = fs_init();
    if (res != 0)
        bugcheck("FS_INIT", (u32*)&res, 1);

    if (bootenv != 0)
        load_boot_firm();

    anim_count = fs_search(BAX_PATH, BAX_FILE, anim_paths, MAX_ANIMATIONS);
    hid_scan();
    if ((anim_count > 0) && !(hid_down() & HID_X))
    {
        int anim_valid;
        size_t anim_sz;
        char anim_path[MAX_PATH], *anim;

        strcpy(anim_path, BAX_PATH "/");
        strcat(anim_path, anim_paths[rnd % anim_count]);

        anim_sz = fs_size(anim_path);
        if (anim_sz > ANIM_MAX_SIZE)
            bugcheck("ANIM_TOO_LARGE", (u32*)&anim_sz, 1);

        anim = malloc(anim_sz);
        if (anim == NULL)
            bugcheck("ANIM_MALLOC", (u32*)&anim_sz, 1);

        fs_read(anim_path, anim, anim_sz);

        anim_valid = anim_validate((anim_t*)anim, anim_sz);
        if (anim_valid != ANIM_OK)
            bugcheck("ANIM_NOT_OK", (u32[]){anim_sz, anim_valid}, 2);

        anim_play((anim_t*)anim);
        free(anim);
    }

    for (int i = 0; i < anim_count; i++)
        free(anim_paths[i]);

    do
    {
        hid_scan();
    } while(hid_down() & HID_X);

    load_boot_firm();
    while(1) _wfi();
}
