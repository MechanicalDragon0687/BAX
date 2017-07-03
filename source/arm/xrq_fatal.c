#include <common.h>
#include <gfx/text.h>

const char *xrq_names[] = {
    "RESET", "UNDEFINED", "SWI",
    "PREFETCH", "DATA", "RESERVED", "IRQ", "FIQ"
};

void fatal_xrq(int xrq_n, uint32_t *regs)
{
    int i, ri;
    char regdump[11][TEXT_WIDTH];

    memset(regdump, 0, 11*TEXT_WIDTH);
    strcpy(regdump[0], xrq_names[xrq_n]);

    for (i = 0; i < 8; i++) {
        ri = i*2;
        msprintf(regdump[i+2], "R%2d = %8X | R%2d = %8X", ri, regs[ri], ri + 1, regs[ri + 1]);
    }
    msprintf(regdump[10], "PSR = %8X | XPSR = %8X", regs[16], regs[17]);
    show_text(regdump, 11);
    return;
}
