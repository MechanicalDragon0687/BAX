#include <common.h>
#include <string.h>

#include <arm/asm.h>
#include <arm/ipc.h>
#include <i2c/i2c.h>

vu32 *a11_entry = (vu32*)(0x1FFFFFF8);
vu32 *a11_vars  = (vu32*)(0x1FFFFFD8);
vu32 *cakehaxfb = (vu32*)(0x23FFFE00);

void *a11_bin_mem = (void*)(0x1FF80000);

static bool ipc_init = false;

bool ipc_available(void)
{
    return ipc_init;
}

void init11(void)
{
    a11_entry[0] = (u32)a11_loop;
    while(a11_entry[0]);
    // Wait for the entrypoint to be cleared (code is executing)

    memcpy(a11_bin_mem, a11bin, a11bin_len);
    a11_entry[0] = (u32)a11_bin_mem;
    while(a11_entry[0]);
    wait11();
    // Wait for both the entrypoint and arguments to be cleared

    ipc_init = true;
}

void screen_init(u32 mode)
{
    if (!ipc_init) return;
    a11_vars[1] = mode;
    a11_vars[0] = 1;
    wait11();

    i2cWriteRegister(0x03, 0x22, 0x2A); // initialize backlight
}

void vramfill(u32 start, u32 end, u32 val)
{
    if (!ipc_init) return;
    a11_vars[1] = start;
    a11_vars[2] = end;
    a11_vars[3] = val;
    a11_vars[0] = 2;
    wait11();
}

u32 play_mjpeg(void *dest, void *src, u32 len)
{
    if (!ipc_init) return 0xDEAD;
    flush_all_caches();
    a11_vars[1] = (u32)(dest);
    a11_vars[2] = (u32)(src);
    a11_vars[3] = len;
    a11_vars[0] = 3;
    wait11();

    return a11_vars[1];
}
