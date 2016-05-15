#pragma once

#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint8_t      u8;
typedef uint16_t     u16;
typedef uint32_t     u32;
typedef uint64_t     u64;

typedef volatile u8          vu8;
typedef volatile u16         vu16;
typedef volatile u32         vu32;
typedef volatile u64         vu64;

#define KEY_A			(1 << 0)  // A
#define KEY_B			(1 << 1)  // B
#define KEY_X			(1 << 10) // X
#define KEY_Y			(1 << 11) // Y
#define KEY_SELECT		(1 << 2)  // SELECT
#define KEY_START		(1 << 3)  // START
#define KEY_DRRIGHT		(1 << 4)  // DPAD RIGHT
#define KEY_DLEFT		(1 << 5)  // DPAD LEFT
#define KEY_DUP			(1 << 6)  // DPAD UP
#define KEY_DDOWN		(1 << 7)  // DPAD DOWN
#define KEY_RT			(1 << 8)  // RIGHT TRIGGER
#define KEY_LT			(1 << 9)  // LEFT TRIGGER

#define CFG_BOOTENV     (*(vu8 *)0x10010000) // CFG_BOOTENV
#define REG_PRNG        (*(vu32*)0x10011000) // REG_PRNG
#define REG_TM0VAL      (*(vu16*)0x10003000) // TIMER0_VAL
#define REG_TM0CNT      (*(vu16*)0x10003002) // TIMER0_CNT
#define HID_PAD         (*(vu32*)0x10146000 ^ 0xFFF) // HID_PAD

#include "draw.h"
#include "fatfs/ff.h"
#include "fs.h"
#include "quicklz.h"
