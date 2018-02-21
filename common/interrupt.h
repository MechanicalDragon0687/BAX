#ifndef INTERRUPT_H
#define INTERRUPT_H

#ifdef ARM9

    #define IRQ_DMA0               (0)
    #define IRQ_DMA1               (1)
    #define IRQ_DMA2               (2)
    #define IRQ_DMA3               (3)
    #define IRQ_DMA4               (4)
    #define IRQ_DMA5               (5)
    #define IRQ_DMA6               (6)
    #define IRQ_DMA7               (7)
    #define IRQ_TIMER0             (8)
    #define IRQ_TIMER1             (9)
    #define IRQ_TIMER2             (10)
    #define IRQ_TIMER3             (11)
    #define IRQ_PXI_SYNC           (12)
    #define IRQ_PXI_RECV_NOT_EMPTY (14)
    #define IRQ_SDIO1              (16)
    #define IRQ_SDIO1_ASYNC        (17)
    #define IRQ_SDIO3              (18)
    #define IRQ_SDIO3_ASYNC        (19)

    #define IRQ_COUNT              (29)

#else

    #define IRQ_TIMER              (0x1D)
    #define IRQ_WATCHDOG           (0x1E)
    #define IRQ_PSC0               (0x28)
    #define IRQ_PSC1               (0x29)
    #define IRQ_VBLANK0            (0x2A)
    #define IRQ_VBLANK1            (0x2B)
    #define IRQ_PXI_SYNC           (0x50)
    #define IRQ_PXI_RECV_NOT_EMPTY (0x53)
    #define IRQ_PDN                (0x58)
    #define IRQ_MCU_HID            (0x71)

    #define IRQ_COUNT              (0x80)

    #define IRQ_SPURIOUS           (0x3FF)

    #define IRQ_VBLANK_FREQ     (60)

#endif // ARM9

#endif // INTERRUPT_H
