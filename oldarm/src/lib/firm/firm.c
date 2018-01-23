#include <common.h>

#include "lib/firm/firm.h"

const u32 _firm_wlist[][2] =
{
    {0x08000040, 0x08100000},
    {0x18000000, 0x18300000},
    {0x1FF00000, 0x1FFFFC00},
    {0x20000000, 0x28000000}
};
const u32 _firm_wlist_regions = 4;

int firm_validate(firm_t *firm, size_t firm_sz)
{
    firm_section_t *sect;
    bool oldarm_entry = false;
    if (firm == NULL)
        return FIRM_MEM_ERR;

    if (memcmp(firm->magic, FIRM_MAGIC, sizeof(firm->magic)))
        return FIRM_BAD_MAGIC;

    if (firm_sz >= FIRM_MAX_SIZE)
        return FIRM_BAD_SIZE;

    for (u32 i = 0; i < FIRM_SECTIONS; i++)
    {
        sect = &firm->section[i];
        u32 ldaddr, offset, size, addr;
        ldaddr = sect->load_addr,
        offset = sect->offset,
        size = sect->size;

        if (size == 0)
            continue;

        if (oldarm_entry == false && 
            ldaddr <= firm->oldarm_entry &&
            (ldaddr + size) > firm->oldarm_entry)
            oldarm_entry = true;

        if (offset & 0x1FF)
            return FIRM_SECT_OFFSET;

        if (ldaddr & 0xF)
            return FIRM_SECT_LDADDR;

        if (size & 0x1FF)
            return FIRM_SECT_SIZE;

        if ((offset + size) > firm_sz)
            return FIRM_BAD_SIZE;

        // Check load address
        for (addr = 0; addr < _firm_wlist_regions; addr++)
        {
            if (ldaddr >= _firm_wlist[addr][0] &&
                (ldaddr + size) < _firm_wlist[addr][1])
                break;
        }
        if (addr == _firm_wlist_regions)
            return FIRM_SECT_LDADDR;

        // TODO: SHA check
    }

    if (oldarm_entry == false)
        return FIRM_OLDARM_ENTRY;

    return FIRM_OK;
}
