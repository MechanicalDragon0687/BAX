#pragma once

#include <common.h>

#define MPCORE_ENTRY    (*(vu32*)(0x1FFFFFFC))

typedef struct
{
    u32 bootenv;
    u32 rndseed;
} sysinfo_t;
