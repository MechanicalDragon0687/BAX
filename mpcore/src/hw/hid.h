#pragma once

#include <common.h>

#define HID_BASE ((vu32*)0x10146000)

typedef enum {
    HID_A      = 0x000,
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
} hid_keys;

// Reinitialize the HID state
void hid_reset(void);

// Update the HID state
void hid_scan(void);

// Keys down (first state pressed)
u32 hid_down(void);

// Keys held (>1 state pressed)
u32 hid_held(void);

// Keys up (pressed in previous state and not in current)
u32 hid_up(void);
