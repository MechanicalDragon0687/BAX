#ifndef FIRM_H
#define FIRM_H

#include <types.h>

#define FIRM_MAGIC    ((u8[4]){'F', 'I', 'R', 'M'})
#define FIRM_MAX_SIZE (0x400000)
#define FIRM_SECTIONS (4)

enum {
    FIRM_OK = 0,
    FIRM_BAD_MAGIC,
    FIRM_BAD_SIZE,
    FIRM_MPCORE_ENTRY,
    FIRM_OLDARM_ENTRY,
    FIRM_SECT_OFFSET,
    FIRM_SECT_LDADDR,
    FIRM_SECT_SIZE,
    FIRM_SECT_HASH
};

typedef struct {
    u32 Offset;
    u32 LoadAddress;
    u32 Length;
    u32 CopyMethod;
    u8  SecureHash[0x20];
} FSect;

typedef struct {
    u8  Signature[4];
    u32 Priority;
    u32 EntryNew;
    u32 EntryOld;
    u8  __rsrvd[0x30];
    FSect Section[FIRM_SECTIONS];
    u8  SecureSignature[0x100];
} FIRM;

int  FIRM_Validate(FIRM *f, size_t sz);
void FIRM_Boot(FIRM *f, const char *path);

#endif
