#include <common.h>

#include <arm/arm.h>

extern char _itcm_lma, _itcm_len, _dtcm_lma, _dtcm_len, _sbss, _ebss;
extern char *fake_heap_start, *fake_heap_end;
static void *_itcm = (void*)0x00000000, *_dtcm = (void*)0x28008000;

void flush_all(void)
{
    flush_icache();
    flush_dcache();
    dwb();
    return;
}

void init_mem(void)
{
    memcpy(_itcm, &_itcm_lma, (size_t)&_itcm_len);
    memcpy(_dtcm, &_dtcm_lma, (size_t)&_dtcm_len);

    memset(&_sbss, 0, &_ebss - &_sbss);
    /* clear .bss */
    return;
}

void init_heap(void)
{
    fake_heap_start = (char*)0x24000000;
    fake_heap_end = (char*)0x28000000;
    return;    
}
