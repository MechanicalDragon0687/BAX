#pragma once

#include <common.h>

#define NDMA_GLOBAL_CNT ((volatile uint32_t*)(0x10002000))
#define NDMA_REGS_BASE  (0x10002004)
#define NDMA_CHANNEL    (7)

#define REG_NDMA_SRC_ADDR     (0x00)
#define REG_NDMA_DST_ADDR     (0x01)
#define REG_NDMA_TRANSFER_CNT (0x02)
#define REG_NDMA_WRITE_CNT    (0x03)
#define REG_NDMA_BLOCK_CNT    (0x04)
#define REG_NDMA_FILL_DATA    (0x05)
#define REG_NDMA_CNT          (0x06)

#define NDMA_DST_INC (0 << 10)
#define NDMA_DST_DEC (1 << 10)
#define NDMA_DST_FIX (2 << 10)
#define NDMA_DST_MSK (BIT(10) | BIT(11))

#define NDMA_SRC_INC (0 << 13)
#define NDMA_SRC_DEC (1 << 13)
#define NDMA_SRC_FIX (2 << 13)
#define NDMA_SRC_FIL (3 << 13)
#define NDMA_SRC_MSK (BIT(13) | BIT(14))

#define NDMA_IMM_MODE   (1 << 28)
#define NDMA_REP_MODE   (1 << 29)
#define NDMA_IRQ_ENABLE (1 << 30)
#define NDMA_START      (1 << 31)

void dma_op(uint32_t dst, uint32_t src, size_t len, int transfer_flags);
