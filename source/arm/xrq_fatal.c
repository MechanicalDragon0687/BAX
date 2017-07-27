#include <common.h>
#include <arm/arm.h>
#include <gfx/text.h>

#define BKPT_OP   (0xE127FF7F)

static bool xrq_trig = false;
const char *xrq_names[] = {
    "Reset", "Undefined", "Software Interrupt",
    "Prefetch abort", "Data abort", "Reserved", "IRQ", "FIQ"
};

void xrq_fatal(int xrq_n, uint32_t *regs)
{
    int i, ri;
    uint32_t op;
    char regdump[11][TEXT_WIDTH];

    memset(regdump, 0, sizeof(regdump));
    strcpy(regdump[0], xrq_names[xrq_n]);

    xrq_trig = !xrq_trig;
    if (xrq_trig == false) {
        strcat(regdump[0], " (double fault >:3) ");
    }

    if ((xrq_trig) && (xrq_n == 3) && (!(regs[REGDUMP_CPSR] & SR_TMB_BIT))
        && (addr_is_exec(regs[REGDUMP_R15]))) {
        op = *(uint32_t*)(regs[REGDUMP_R15]);
        if (op == BKPT_OP) {
            strcat(regdump[0], " (bkpt)");
        }
    }

    for (i = 0; i < 8; i++) {
        ri = i*2;
        msprintf(regdump[i+2], "R%2d = %8X | R%2d = %8X",
                 ri, regs[ri], ri + 1, regs[ri + 1]);
    }

    msprintf(regdump[10], "PSR = %8X | XPSR = %8X",
             regs[REGDUMP_CPSR], regs[REGDUMP_XPSR]);

    show_text(regdump, 11);
    return;
}
