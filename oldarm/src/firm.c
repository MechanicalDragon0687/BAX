#include <common.h>
#include <firm.h>
#include <hw/sha.h>

u32 wlist[][2] = {
    {0x08000000, 0x08100000},
    {0x18000000, 0x18600000},
    {0x1FF00000, 0x1FFFFC00},
    {0x20000000, 0x20000000},
    {0x23400000, 0x23FFFE00},
    {0x24000000, 0x27FFFD00}
};

static inline bool range(u32 x, u32 a, u32 b)
{ return (x>=a&&x<=b); }

bool firm_check_address(u32 loc, u32 dest, u32 size)
{
    bool ret=false;
    u32 regs = sizeof(wlist)/sizeof(wlist[0]);
    if (loc&0x1FF || dest&0x3 || size&0x1FF)
        return false;

    for (u32 i = 0; (ret == false) && (i < regs); i++) {
        ret |= (range(dest, wlist[i][0], wlist[i][1]) &&
                range(dest+size, wlist[i][0], wlist[i][1]));
    }
    return ret;
}

bool firm_check(void *firm, u32 firm_size)
{
    firm_header *hdr;
    firm_section *sect;
    bool mpcore_avail, oldarm_avail;
    u32 fs_size;
    u32 sect_hash[0x20/4];

    hdr = (firm_header*)firm;
    if (firm_size < sizeof(firm_header) ||
        memcmp(hdr->magic, FIRM_MAGIC, 4)) {
        return false;
    }

    mpcore_avail = false;
    oldarm_avail = false;
    fs_size = sizeof(firm_header);

    for (int i = 0; i < 4; i++) {
        u32 off, addr, size;
        sect = &(hdr->section[i]);
        off = sect->off;
        addr = sect->addr;
        size = sect->size;

        if (!size)
            continue;

        fs_size += size;
        if (fs_size > firm_size || !firm_check_address((u32)hdr + off, addr, size))
            return false;

        sha(sect_hash, (void*)((u32)hdr + off), size, SHA256);
        if (memcmp(sect_hash, sect->sha_hash, 0x20)) {
            return false;
        }

        if (!mpcore_avail && range(hdr->mpcore, addr, addr + size))
            mpcore_avail = true;

        if (!oldarm_avail && range(hdr->oldarm, addr, addr + size))
            oldarm_avail = true;
    }

    if (!mpcore_avail)
        hdr->mpcore = 0;

    return oldarm_avail;
}

