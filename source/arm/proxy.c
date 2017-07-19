#include <common.h>
#include <arm/cpu.h>
#include <arm/proxy.h>

void *proxy_tgt = (void*)(0x1FFFF800);
volatile uint32_t *proxy_entry = (volatile uint32_t*)(0x1FFFFFF8);

void proxy_boot(uint32_t addr)
{
    proxy_entry[0] = addr;
    proxy_entry[1] = addr;
    while(proxy_entry[0] && proxy_entry[1]);
}

void proxy_init(void)
{
    memcpy(proxy_tgt, IPC_BIN_LOC, IPC_BIN_LEN);
    writeback_invalidate_dcache_range((uint32_t)proxy_tgt, (uint32_t)proxy_tgt + IPC_BIN_LEN);
    proxy_boot((uint32_t)proxy_tgt);
    return;
}
