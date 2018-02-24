#include <common.h>

#include "hw/sha.h"
#include "lib/firm/firm.h"

static const u8  FIRM_Signature[4] = {'F', 'I', 'R', 'M'};
static const u32 FIRM_MemWhiteList[][2] =
{
    {0x08000040, 0x08100000},
    {0x18000000, 0x18300000},
    {0x1FF00000, 0x1FFFFC00},
    {0x20000000, 0x28000000}
}, FIRM_MemWhiteListRegions = sizeof(FIRM_MemWhiteList) / sizeof(*FIRM_MemWhiteList);

int FIRM_Validate(FIRM *f, size_t sz)
{
    u8 *fv = (u8*)f;
    FSect *s;
    bool OldEntryFound = false;

    assert(f != NULL);

    if (memcmp(f->Signature, FIRM_Signature, sizeof(f->Signature)))
        return FIRM_BAD_MAGIC;

    if (sz >= FIRM_MAX_SIZE)
        return FIRM_BAD_SIZE;

    for (u32 i = 0; i < FIRM_SECTIONS; i++) {
        s = &f->Section[i];
        u32 addr, sha_hash[SHA_HashSizeWord(SHA_MODE_256)];

        if (s->Length == 0)
            continue;

        if ((OldEntryFound == false) &&
            bound(f->EntryOld, s->LoadAddress, s->LoadAddress + s->Length))
            OldEntryFound = true;

        if (s->Offset & 0x1FF)
            return FIRM_SECT_OFFSET;

        if (s->LoadAddress & 0xF)
            return FIRM_SECT_LDADDR;

        if (s->Length & 0x1FF)
            return FIRM_SECT_SIZE;

        if ((s->Offset + s->Length) > sz)
            return FIRM_BAD_SIZE;

        for (addr = 0; addr < FIRM_MemWhiteListRegions; addr++) {
            if (bound(s->LoadAddress,
                FIRM_MemWhiteList[addr][0],
                FIRM_MemWhiteList[addr][1]))
                break;
        }
        if (addr == FIRM_MemWhiteListRegions)
            return FIRM_SECT_LDADDR;

        SHA_CalculateHash((u32*)(fv + s->Offset), s->Length, SHA_MODE_256, sha_hash);
        if (memcmp(s->SecureHash, sha_hash, sizeof(s->SecureHash)) != 0)
            return FIRM_SECT_HASH;
    }

    if (OldEntryFound == false)
        return FIRM_OLDARM_ENTRY;

    return FIRM_OK;
}
