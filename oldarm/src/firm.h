#pragma once
#include <common.h>

#define FIRM_MAGIC ((u8[]){'F', 'I', 'R', 'M'})

typedef struct {
    u32 off;
    u32 addr;
    u32 size;
    u32 copy_meth;
    u32 sha_hash[8];
} firm_section;

typedef struct {
    u8 magic[4];
    u32 prio;
    u32 mpcore;
    u32 oldarm;
    u8 rsrvd[0x30];
    firm_section section[4];
    u8 rsa_sig[0x100];
} firm_header;

bool firm_check_address(u32 loc, u32 dest, u32 size);
bool firm_check(void *firm, u32 firm_size);
