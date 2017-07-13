#include <common.h>
#include <arm/cpu.h>

extern int _itcm_lma, _itcm_len, _dtcm_lma, _dtcm_len, _sbss, _ebss;
extern int _itcm_loc, _dtcm_loc;

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
