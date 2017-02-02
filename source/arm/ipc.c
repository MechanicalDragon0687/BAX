#include <common.h>
#include <string.h>

#include <arm/asm.h>
#include <arm/ipc.h>

#include <i2c/i2c.h>

volatile uint32_t *a11_entry = (volatile uint32_t*)(0x1FFFFFF8);
volatile uint32_t *a11_vars  = (volatile uint32_t*)(0x1FFF4C70);
volatile uint32_t *cakehaxfb = (volatile uint32_t*)(0x23FFFE00);

uint32_t *a11_bin_mem = (uint32_t*)(0x1FFF4C80);

static bool ipc_init = false;

void init11(void)
{
    a11_entry[0] = (uint32_t)a11_loop;
    flush_all_caches();
    while(a11_entry[0]);

    memcpy(a11_bin_mem, a11bin, a11bin_len);
    a11_entry[0] = (uint32_t)a11_bin_mem;
    flush_all_caches();
    while(a11_entry[0]);
    wait11();

    ipc_init = true;
}

void vramfill(uint32_t start, uint32_t end, uint32_t val)
{
    if (!ipc_init) return;
    a11_vars[1] = start;
    a11_vars[2] = end;
    a11_vars[3] = val;
    a11_vars[0] = 2;

    flush_all_caches();
    wait11();
}

void screen_init(uint32_t mode)
{
    if (!ipc_init) return;
    a11_vars[1] = mode;
    a11_vars[0] = 1;

    flush_all_caches();
    wait11();

    i2cWriteRegister(0x03, 0x22, 0x2A);
}
