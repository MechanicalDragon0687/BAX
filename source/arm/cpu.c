#include <common.h>
#include <arm/cpu.h>

extern void *fake_heap_start, *fake_heap_end;

/* Initialize ITCM, DTCM and clear BSS */
void init_mem(void)
{
    memcpy(&_itcm_loc, &_itcm_lma, (size_t)&_itcm_len);
    memcpy(&_dtcm_loc, &_dtcm_lma, (size_t)&_dtcm_len);
    memset(&_sbss, 0, &_ebss - &_sbss);
    return;
}

/* Initialize newlib heap */
void init_heap(void)
{
    fake_heap_start = (void*)0x24000000;
    fake_heap_end = (void*)0x28000000;
    return;
}
