#include <common.h>
#include "hw/hid.h"

static u32 kprev, kcur;

void hid_reset(void)
{
    kprev = 0;
    kcur = 0;
    return;
}

void hid_scan(void)
{
    kprev = kcur;
    kcur = ~(*HID_BASE) & HID_ANY;
    return;
}

u32 hid_down(void)
{
    return kcur;
}

u32 hid_held(void)
{
    return kprev & kcur;
}

u32 hid_up(void)
{
    return kprev & ~kcur;
}
