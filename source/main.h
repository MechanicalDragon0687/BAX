#pragma once

#define	PAYLOAD_ADDR    0x24F00000

#define HID_PAD         (*(vu16 *)0x10146000 ^ 0xFFF)
#define BUTTON_SELECT   (1 << 2)
#define BUTTON_R1       (1 << 8)
