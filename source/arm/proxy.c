#include <common.h>
#include <arm/cpu.h>
#include <arm/ipc.h>

void *proxy_tgt = (void*)(0x1FFFFC00);
volatile uint32_t *proxy_entry = (volatile uint32_t*)(0x1FFFFFF8);

void proxy_boot(uint32_t addr)
{
    proxy_entry[0] = addr;
    proxy_entry[1] = addr;
    while((proxy_entry[0] != 0) && (proxy_entry[1] != 0));
}

void proxy_init(void)
{
    memcpy(proxy_tgt, ipc_bin, ipc_bin_len);
    writeback_invalidate_dcache_range((uint32_t)proxy_tgt, (uint32_t)proxy_tgt + ipc_bin_len);
    proxy_boot((uint32_t)proxy_tgt);
    return;
}
