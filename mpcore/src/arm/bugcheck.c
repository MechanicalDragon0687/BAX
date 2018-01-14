#include <common.h>
#include <cache.h>
#include <cpu.h>

#include "hw/gx.h"
#include "hw/lcd.h"
#include "console.h"

extern const gx_framebuffers_t _default_framebuffers;
void _bugcheck(const char *str)
{
    _disable_irqs();
    _writeback_invalidate_DC();
    _invalidate_IC();

    gx_select_buffers(0);
    gx_set_framebuffers(&_default_framebuffers);
    lcd_stop_fill();

    console_reset((u16*)_default_framebuffers.addr[0][0], 400);
    console_puts(str);
    while(1) _wfi();
}

/*
 Order of register is as follows:
 - R0-R15
 - CPSR, XPSR
 - TTBR, DACR
 - DFSR, IFSR, FAR
*/
const int _bugcheck_reg_count = 23;
const char *_bugcheck_reg_name[] =
{
    "R00", "R01", "R02", "R03", "R04", "R05", "R06",
    "R07", "R08", "R09", "R10", "R11", "R12", "R13",
    "R14", "R15", "CPSR", "XPSR", "TTBR", "DACR",
    "DFSR", "IFSR", "FAR"
};

void _bugcheck_xrq(int xrq, u32 *regs)
{
    char str[1024] = "  nawt gud!\n";
    str[0] = xrq + '0';
    _bugcheck(str);
}
