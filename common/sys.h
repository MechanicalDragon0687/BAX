#pragma once

#include <common.h>

#define ENTRYPOINT_FIRM (0)
#define MPCORE_ENTRY    (*(vu32*)(0x1FFFFFFC))

typedef struct
{
    u32 sysprot;
    u32 bootenv;
    u32 rndseed;
    u32 entrypn;
} sysinfo_t;
