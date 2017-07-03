#include <common.h>
#include <arm/cpu.h>

extern void *fake_heap_start, *fake_heap_end;

void init_mem(void)
{
    memcpy(&_itcm_loc, &_itcm_lma, (size_t)&_itcm_len);
    memcpy(&_dtcm_loc, &_dtcm_lma, (size_t)&_dtcm_len);
    memset(&_sbss, 0, &_ebss - &_sbss);
    return;
}

void init_heap(void)
{
    fake_heap_start = (void*)0x24000000;

    //if (N3DS) fake_heap_end = (void*)0x30000000; else

    fake_heap_end = (void*)0x28000000;
    return;
}
