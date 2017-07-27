#include <common.h>
#include <arm/cpu.h>
#include <hw/dma.h>

volatile uint32_t (*dma_regs)[7] = (volatile uint32_t(*)[7])(NDMA_REGS_BASE);

/* Initialize NDMA */
void dma_init(void)
{
    *NDMA_GLOBAL_CNT = 1;
    return;
}

/* Wait until channel is not busy */
void dma_wait(unsigned int channel)
{
    while(dma_regs[channel][REG_NDMA_CNT] & NDMA_START);
    return;
}

void dma_op(uint32_t dst, uint32_t src, size_t len, int transfer_flags)
{
    uint32_t dst_e, src_e;

    /* Alignment checks */
    if ((dst|src|len)&3 || len < 4) {
        abort();
    }

    /* Calculate transfer boundaries */
    switch(transfer_flags & NDMA_DST_MSK) {
    case NDMA_DST_INC:
        dst_e = dst + len;
        break;
    case NDMA_DST_DEC:
        dst_e = dst - len;
        break;
    default:
        dst_e = dst;
        break;
    }

    switch(transfer_flags & NDMA_SRC_MSK) {
    case NDMA_SRC_INC:
        src_e = src + len;
        break;
    case NDMA_SRC_DEC:
        src_e = src - len;
        break;
    default:
        src_e = src;
        break;
    }

    /*
     Source is cacheable
     - write back DCache and drain the write buffer
    */
    if ((!(transfer_flags & NDMA_SRC_FIL)) && addr_is_cached(src)) {
        wb_dc_range(src, src_e);
        drain_write_buffer();
    }

    /* Stop any possible transfer on this channel */
    dma_regs[NDMA_CHANNEL][REG_NDMA_CNT] = 0;

    dma_regs[NDMA_CHANNEL][REG_NDMA_SRC_ADDR] = src;
    dma_regs[NDMA_CHANNEL][REG_NDMA_DST_ADDR] = dst;
    dma_regs[NDMA_CHANNEL][REG_NDMA_TRANSFER_CNT] = 0;
    dma_regs[NDMA_CHANNEL][REG_NDMA_WRITE_CNT] = len/4;
    dma_regs[NDMA_CHANNEL][REG_NDMA_BLOCK_CNT] = 0;

    if (transfer_flags & NDMA_SRC_FIL) {
        dma_regs[NDMA_CHANNEL][REG_NDMA_FILL_DATA] = src;
    }

    dma_regs[NDMA_CHANNEL][REG_NDMA_CNT] = transfer_flags | NDMA_START;
    dma_wait(NDMA_CHANNEL);

    /*
     Destination is cacheable
     - invalidate caches
    */
    if (addr_is_cached(dst)) {
        inv_ic_range(dst, dst_e);
        inv_dc_range(dst, dst_e);
    }

    return;
}
