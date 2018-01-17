#include <common.h>
#include <pxicmd.h>
#include <cache.h>
#include <sys.h>

#include "lib/firm/firm.h"

const u32 _firm_wlist[][2] =
{
    {0x08000040, 0x08100000},
    {0x18000000, 0x18600000},
    {0x1FF00000, 0x1FFFFC00},
    {0x20000000, 0x28000000}
};
const u32 _firm_wlist_regions = sizeof(_firm_wlist)/sizeof(u32[2]);

const char *_firm_err_str[] =
{
    [FIRM_OK]           = "No error",
    [FIRM_MEM_ERR]      = "Null pointer",
    [FIRM_BAD_MAGIC]    = "Not a FIRM",
    [FIRM_BAD_SIZE]     = "Invalid size",
    [FIRM_MPCORE_ENTRY] = "Invalid ARM11 entrypoint",
    [FIRM_OLDARM_ENTRY] = "Invalid ARM9 entrypoint",
    [FIRM_SECT_OFFSET]  = "Invalid section offset",
    [FIRM_SECT_LDADDR]  = "Invalid section load address",
    [FIRM_SECT_SIZE]    = "Invalid section size",
    [FIRM_SECT_HASH]    = "Section SHA256 mismatch"
};

int firm_validate(firm_t *firm, size_t firm_sz)
{
    firm_section_t *sect;
    bool mpcore_entry = false, oldarm_entry = false;
    if (firm == NULL)
        return FIRM_MEM_ERR;

    if (memcmp(firm->magic, FIRM_MAGIC, sizeof(firm->magic)))
        return FIRM_BAD_MAGIC;

    if (firm_sz >= FIRM_MAX_SIZE)
        return FIRM_BAD_SIZE;

    for (u32 i = 0; i < FIRM_SECTIONS; i++)
    {
        sect = &firm->section[i];
        u32 addr;

        if (sect->size == 0)
            continue;

        if (mpcore_entry == false && 
            sect->load_addr <= firm->mpcore_entry &&
            (sect->load_addr + sect->size) > firm->mpcore_entry)
            mpcore_entry = true;

        if (oldarm_entry == false && 
            sect->load_addr <= firm->oldarm_entry &&
            (sect->load_addr + sect->size) > firm->oldarm_entry)
            oldarm_entry = true;

        if (sect->offset & 0x1FF)
            return FIRM_SECT_OFFSET;

        if (sect->load_addr & 0x1FF)
            return FIRM_SECT_LDADDR;

        if (sect->size & 0x1FF)
            return FIRM_SECT_SIZE;

        if ((sect->offset + sect->size) > firm_sz)
            return FIRM_BAD_SIZE;

        // Check load address
        for (addr = 0; addr < _firm_wlist_regions; addr++)
        {
            if (sect->load_addr >= _firm_wlist[addr][0] &&
                (sect->load_addr + sect->size) < _firm_wlist[addr][1])
                break;
        }
        if (addr == _firm_wlist_regions)
            return FIRM_SECT_LDADDR;

        // TODO: SHA check
    }

    if (mpcore_entry == false)
        return FIRM_MPCORE_ENTRY;

    if (oldarm_entry == false)
        return FIRM_OLDARM_ENTRY;

    return FIRM_OK;
}

void NORETURN firm_boot(firm_t *firm)
{
    pxicmd_send(PXICMD_ARM11_STUBOUT, NULL, 0);

    for (int sect = 0; sect < FIRM_SECTIONS; sect++)
    {
        memcpy((void*)firm->section[sect].load_addr,
            (void*)firm + firm->section[sect].offset,
            firm->section[sect].size);
    }

    _disable_irqs();
    _writeback_invalidate_DC();
    _invalidate_IC();
    _write_CR((_read_CR() & ~(0x1005)) | 0x2000);

    MPCORE_ENTRY = firm->mpcore_entry;
    __asm__ __volatile__(
        "bx %0\n\t"
        : : "r"(firm->oldarm_entry) : "memory"
    );

    while(1) _wfi();
}
