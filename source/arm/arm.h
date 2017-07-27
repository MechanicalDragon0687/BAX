#pragma once

/* Status Register flags */
#define SR_USR_MODE (0x10)
#define SR_FIQ_MODE (0x11)
#define SR_IRQ_MODE (0x12)
#define SR_SVC_MODE (0x13)
#define SR_ABT_MODE (0x17)
#define SR_UND_MODE (0x1B)
#define SR_SYS_MODE (0x1F)
#define SR_PMODE_MASK (0x1F)

#define SR_TMB_BIT  (1 << 5)
#define SR_FIQ_BIT  (1 << 6)
#define SR_IRQ_BIT  (1 << 7)

/* Control Register flags */
#define CR_ENABLE_MPU    (1 << 0)
#define CR_ENABLE_BIGEND (1 << 7)
#define CR_ENABLE_DCACHE (1 << 2)
#define CR_ENABLE_ICACHE (1 << 12)
#define CR_ENABLE_DTCM   (1 << 16)
#define CR_ENABLE_ITCM   (1 << 18)
#define CR_ALT_VECTORS   (1 << 13)
#define CR_CACHE_RROBIN  (1 << 14)
#define CR_DISABLE_TBIT  (1 << 15)
#define CR_DTCM_LMODE    (1 << 17)
#define CR_ITCM_LMODE    (1 << 19)

/* MPU Region attributes */
#define MPU_4K   (0xB << 1)
#define MPU_8K   (0xC << 1)
#define MPU_16K  (0xD << 1)
#define MPU_32K  (0xE << 1)
#define MPU_64K  (0xF << 1)
#define MPU_128K (0x10 << 1)
#define MPU_256K (0x11 << 1)
#define MPU_512K (0x12 << 1)
#define MPU_1M   (0x13 << 1)
#define MPU_2M   (0x14 << 1)
#define MPU_4M   (0x15 << 1)
#define MPU_8M   (0x16 << 1)
#define MPU_16M  (0x17 << 1)
#define MPU_32M  (0x18 << 1)
#define MPU_64M  (0x19 << 1)
#define MPU_128M (0x1A << 1)
#define MPU_256M (0x1B << 1)
#define MPU_512M (0x1C << 1)
#define MPU_1G   (0x1D << 1)
#define MPU_2G   (0x1E << 1)
#define MPU_4G   (0x1F << 1)

#define MPU_NA_NA (0b0000)
#define MPU_RW_NA (0b0001)
#define MPU_RW_RO (0b0010)
#define MPU_RW_RW (0b0011)
#define MPU_RO_NA (0b0101)
#define MPU_RO_RO (0b0110)

#define MPU_ACCESS(a,b,c,d,e,f,g,h)  ((h)<<28 | \
                                      (g)<<24 | \
                                      (f)<<20 | \
                                      (e)<<16 | \
                                      (d)<<12 | \
                                      (c)<<8 | \
                                      (b)<<4 | \
                                      (a))

/* Fatal XRQ register dump definitions */
#define REGDUMP_R0   (0)
#define REGDUMP_R1   (1)
#define REGDUMP_R2   (2)
#define REGDUMP_R3   (3)
#define REGDUMP_R4   (4)
#define REGDUMP_R5   (5)
#define REGDUMP_R6   (6)
#define REGDUMP_R7   (7)
#define REGDUMP_R8   (8)
#define REGDUMP_R9   (9)
#define REGDUMP_R10  (10)
#define REGDUMP_R11  (11)
#define REGDUMP_R12  (12)
#define REGDUMP_R13  (13)
#define REGDUMP_R14  (14)
#define REGDUMP_R15  (15)
#define REGDUMP_FP   (REGDUMP_R12)
#define REGDUMP_SP   (REGDUMP_R13)
#define REGDUMP_LR   (REGDUMP_R14)
#define REGDUMP_PC   (REGDUMP_R15)
#define REGDUMP_CPSR (16)
#define REGDUMP_XPSR (17)
