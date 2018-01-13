#pragma once

#include <common.h>

#define CONSOLE_O3DS (0)
#define CONSOLE_N3DS (1)

typedef struct
{
	u32 sysprot;
	u32 bootenv;
	u32 rndseed;
} sysinfo_t;
