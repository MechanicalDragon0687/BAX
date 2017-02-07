#pragma once

#include <stdint.h>
#include <stddef.h>

#define HID_PAD (*(vu32*)(0x10146000))

#define KEY_A     (1 << 0)
#define KEY_B     (1 << 1)
#define KEY_SEL   (1 << 2)
#define KEY_START (1 << 3)
#define KEY_RIGHT (1 << 4)
#define KEY_LEFT  (1 << 5)
#define KEY_UP    (1 << 6)
#define KEY_DOWN  (1 << 7)
#define KEY_R     (1 << 8)
#define KEY_L     (1 << 9)
#define KEY_X     (1 << 10)
#define KEY_Y     (1 << 11)
