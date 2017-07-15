#pragma once

#define REG_GPU_CNT  ((volatile uint32_t*)(0x10141200))

#define REG_LCD_CFG(n)  (*(volatile uint32_t*)(0x10202000))

/* GPU fill registers */
#define REG_GPU_PCS0(n) (*(volatile uint32_t*)(0x10400010 + (n)))
#define REG_GPU_PCS1(n) (*(volatile uint32_t*)(0x10400020))

#define REG_PCS_BUFFER_START (0x00)
#define REG_PCS_BUFFER_END   (0x04)
#define REG_PCS_FILL_VALUE   (0x08)
#define REG_PCS_CONTROL      (0x0C)

#define PCS_ADDR(x)      ((x) >> 3)

#define PCS_CNT_START    (1<<0)
#define PCS_CNT_FINISHED (1<<1)

#define PCS_CNT_RGB16    (0<<8)
#define PCS_CNT_RGB24    (1<<8)
#define PCS_CNT_RGB32    (2<<8)

/* GPU LCD configuration registers */
#define REG_GPU_PCD0(n) (*(volatile uint32_t*)(0x10400400 + (n)))
#define REG_GPU_PCD1(n) (*(volatile uint32_t*)(0x10400500 + (n)))

#define FB_TOP_SIZE (0x0005DC00)
#define FB_SUB_SIZE (0x0004B000)

#define VRAM_START  (0x18300000)

#define FB_TOPL_A (VRAM_START)
#define FB_TOPL_B (FB_TOPL_A + FB_TOP_SIZE)
#define FB_TOPR_A (FB_TOPL_B + FB_TOP_SIZE)
#define FB_TOPR_B (FB_TOPR_A + FB_TOP_SIZE)

#define FB_TOP_ST (FB_TOPL_A)
#define FB_TOP_LE (FB_TOP_SIZE * 4)

#define FB_SUB_A  (FB_TOPR_B + FB_TOP_SIZE)
#define FB_SUB_B  (FB_SUB_A + FB_SUB_SIZE)

#define FB_SUB_ST (FB_BOTTOM_A)
#define FB_SUB_LE (FB_SUB_SIZE * 2)

#define VRAM_END  (VRAM_START + FB_TOP_LE + FB_SUB_LE)

#define FB_CFG_RGBA32 (0)
#define FB_CFG_RGB24  (1)
#define FB_CFG_RGB565 (2)
#define FB_CFG_RGB5A1 (3)

#define FB_CFG_ENABLE_PARALLAX (1<<5)
#define FB_CFG_MAIN_SCREEN     (1<<6)
