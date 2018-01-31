#pragma once

#include <common.h>

#define FIRM_MAGIC    ((u8[4]){'F', 'I', 'R', 'M'})
#define FIRM_MAX_SIZE (0x400000)
#define FIRM_SECTIONS (4)

enum {
    FIRM_OK = 0,
    FIRM_MEM_ERR,
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
    u32 offset;
    u32 load_addr;
    u32 size;
    u32 copy_method;
    u8  sha_hash[0x20];
} firm_section_t;

typedef struct {
    u8  magic[4];
    u32 priority;
    u32 mpcore_entry;
    u32 oldarm_entry;
    u8  reserved[0x30];
    firm_section_t section[FIRM_SECTIONS];
    u8  rsa_sig[0x100];
} firm_t;

int firm_validate(firm_t *firm, size_t firm_sz);
void firm_boot(firm_t *firm, const char *path);
