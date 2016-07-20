#pragma once

#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

typedef int8_t       s8;
typedef int16_t      s16;
typedef int32_t      s32;
typedef int64_t      s64;

typedef uint8_t      u8;
typedef uint16_t     u16;
typedef uint32_t     u32;
typedef uint64_t     u64;

typedef volatile u8  vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;

#define BASE_PATH       "/anim/"
//#define BASE_PATH     "/3ds/arm9/anim/" SOON™

#define TOP_ANIM_PATH   BASE_PATH "0/anim"
#define SUB_ANIM_PATH   BASE_PATH "0/bottom_anim"
#define CFG_ANIM_PATH   BASE_PATH "0/config.txt"
#define PAYLOAD_PATH    BASE_PATH "arm9payload.bin"
#define CALIB_PATH      BASE_PATH "calibrator"

#define KEY_A           (1 << 0)  // A
#define KEY_B           (1 << 1)  // B
#define KEY_X           (1 << 10) // X
#define KEY_Y           (1 << 11) // Y
#define KEY_SELECT      (1 << 2)  // SELECT
#define KEY_START       (1 << 3)  // START
#define KEY_DRIGHT      (1 << 4)  // DPAD RIGHT
#define KEY_DLEFT       (1 << 5)  // DPAD LEFT
#define KEY_DUP         (1 << 6)  // DPAD UP
#define KEY_DDOWN       (1 << 7)  // DPAD DOWN
#define KEY_RT          (1 << 8)  // RIGHT SHOULDER TRIGGER
#define KEY_LT          (1 << 9)  // LEFT SHOULDER TRIGGER

#define KEY_SKIP        (KEY_A | KEY_B | KEY_X | KEY_Y | KEY_SELECT | KEY_START) // Keys to end the animation

#define CFG_BOOTENV     (*(vu8 *)0x10010000) // CFG_BOOTENV
#define PDN_GPU_CNT     (*(vu8 *)0x10141200) // PDN_GPU_CNT
#define REG_PRNG        (*(vu32*)0x10011000) // REG_PRNG
#define REG_TM0VAL      (*(vu16*)0x10003000) // TIMER0_VAL
#define REG_TM0CNT      (*(vu16*)0x10003002) // TIMER0_CNT
#define HID_PAD         (*(vu16*)0x10146000 ^ 0xFFF) // HID_PAD

void error(const char *msg);
void poweroff();
void chainload();

#include "anim.h"
#include "draw.h"
#include "fatfs/ff.h"
#include "fs.h"
#include "gw.h"
#include "i2c.h"
#include "quicklz.h"
