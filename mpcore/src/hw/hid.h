#ifndef HID_H
#define HID_H

#include <types.h>

#define REG_HID_BASE (*((vu16*)0x10146000))

enum {
    HID_A      = 0x001,
    HID_B      = 0x002,
    HID_X      = 0x400,
    HID_Y      = 0x800,
    HID_L      = 0x200,
    HID_R      = 0x100,
    HID_UP     = 0x040,
    HID_DOWN   = 0x080,
    HID_LEFT   = 0x020,
    HID_RIGHT  = 0x010,
    HID_START  = 0x008,
    HID_SELECT = 0x004,
    HID_ANY    = 0xFFF
};

void HID_Reset(void);
void HID_Scan(void);

u32 HID_Down(void);
u32 HID_Held(void);
u32 HID_Up(void);

#endif // HID_H
