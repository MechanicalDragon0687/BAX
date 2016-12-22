#pragma once

#include <stdint.h>
#include <inttypes.h>

uint32_t ctrand()
{
    return *(volatile uint32_t*)(0x10011000);
}
