#include <common.h>
#include <gfx/text.h>
#include <arm/exceptions.h>

const char *xrq_names[] =
{
    "Reset", "Undefined", "Software interrupt",
    "Prefetch abort", "Data abort", "Reserved", "IRQ", "FIQ"
};

void fatal_xrq(int xrq_n, uint32_t *regs)
{
    char regdump[11][TEXT_WIDTH];

    memset(regdump, 0, 11*TEXT_WIDTH);

    strcpy(regdump[0], xrq_names[xrq_n]);

    msprintf(regdump[2], "R00 = %X | R01 = %X", regs[0], regs[1]);
    msprintf(regdump[3], "R02 = %X | R03 = %X", regs[2], regs[3]);
    msprintf(regdump[4], "R04 = %X | R05 = %X", regs[4], regs[5]);
    msprintf(regdump[5], "R06 = %X | R07 = %X", regs[6], regs[7]);
    msprintf(regdump[6], "R08 = %X | R09 = %X", regs[8], regs[9]);
    msprintf(regdump[7], "R10 = %X | R11 = %X", regs[10], regs[11]);
    msprintf(regdump[8], "R12 = %X | R13 = %X", regs[12], regs[13]);
    msprintf(regdump[9], "R14 = %X | R15 = %X", regs[14], regs[15]);

    msprintf(regdump[10], "PSR = %X | XPSR = %X", regs[16], regs[17]);

    show_text(regdump, 11);
    return;
}
