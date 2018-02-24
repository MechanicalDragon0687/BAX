#include <types.h>

#include "hw/hid.h"

static u32 KDown, KHeld, KUp;

void HID_Reset(void)
{
    KDown = 0;
    KHeld = 0;
    KUp = 0;
    return;
}

void HID_Scan(void)
{
    u32 k = ~REG_HID_BASE & HID_ANY;

    KUp   = KHeld & ~k;
    KHeld = KDown & k;
    KDown = k;
    return;
}

u32 HID_Down(void)
{
    return KDown;
}

u32 HID_Held(void)
{
    return KHeld;
}

u32 HID_Up(void)
{
    return KUp;
}
