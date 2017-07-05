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

void dma_copy(void *destination, void *source, size_t len, int transfer_flags)
{
    uint32_t dst, src, dst_e, src_e;

    dst = (uint32_t)destination;
    src = (uint32_t)source;

    /* Alignment checks */
    if ((dst|src|len)&3 || len < 4) {
        abort();
    }

    /* Calculate the end of destination and source */
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
    if (addr_is_cached(src)) {
        writeback_dcache_range(src, src_e);
        drain_write_buffer();
    }

    /* Stop any possible transfer on this channel */
    dma_regs[NDMA_CHANNEL][REG_NDMA_CNT] = 0;

    /*
     TODO: Find a working example of this working with EMMC.
     Hopefully Kernel9 uses it.
    */
    dma_regs[NDMA_CHANNEL][REG_NDMA_SRC_ADDR] = src;
    dma_regs[NDMA_CHANNEL][REG_NDMA_DST_ADDR] = dst;
    dma_regs[NDMA_CHANNEL][REG_NDMA_TRANSFER_CNT] = 0;
    dma_regs[NDMA_CHANNEL][REG_NDMA_WRITE_CNT] = len/4;
    dma_regs[NDMA_CHANNEL][REG_NDMA_BLOCK_CNT] = 0;
    dma_regs[NDMA_CHANNEL][REG_NDMA_CNT] = transfer_flags | NDMA_IMM_MODE | NDMA_START;
    dma_wait(NDMA_CHANNEL);

    /*
     Destination is cacheable
     - invalidate I and DCache
    */
    if (addr_is_cached(dst)) {
        invalidate_icache_range(dst, dst_e);
        invalidate_dcache_range(dst, dst_e);
    }

    return;
}
