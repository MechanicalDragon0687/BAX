#pragma once

#include "types.h"

#define PAYLOAD_ADDR	0x24F00000

#define HID_PAD         (*(vu16 *)0x10146000 ^ 0xFFF)
#define BUTTON_R1       (1 << 8)