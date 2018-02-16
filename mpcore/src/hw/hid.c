#include <types.h>

#include "hw/hid.h"

static u32 KPrev, KCur;

void HID_Reset(void)
{
    KPrev = 0;
    KCur = 0;
    return;
}

void HID_Scan(void)
{
    KPrev = KCur;
    KCur = ~REG_HID_BASE & HID_ANY;
    return;
}

u32 HID_Down(void)
{
    return KCur;
}

u32 HID_Held(void)
{
    return KPrev & KCur;
}

u32 HID_Up(void)
{
    return KPrev & ~KCur;
}
