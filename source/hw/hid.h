#pragma once

#include <common.h>

#define HID_PAD ((volatile uint16_t*)(0x10146000))

enum {
    HID_A       = BIT(0),
    HID_B       = BIT(1),
    HID_X       = BIT(10),
    HID_Y       = BIT(11),
    HID_UP      = BIT(6),
    HID_DOWN    = BIT(7),
    HID_LEFT    = BIT(5),
    HID_RIGHT   = BIT(4),
    HID_START   = BIT(3),
    HID_SELECT  = BIT(2),
    HID_LT      = BIT(9),
    HID_RT      = BIT(8)
};

#define HID_ANY_DPAD (HID_UP | HID_DOWN | HID_LEFT | HID_RIGHT)
#define HID_ANY_FACE (HID_A  | HID_B | HID_X | HID_Y)
#define HID_ANY_TRGR (HID_LT | HID_RT)

static inline uint32_t hid_key_pressed(void)
{
    return (~(*HID_PAD) & 0xFFF);
}
