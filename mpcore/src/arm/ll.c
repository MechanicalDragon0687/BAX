#include <common.h>
#include <vram.h>

#define CACHE_CODE
#include <cache.h>

#define PXI_CODE
#include <pxi.h>

#define IRQ_CODE
#include <irq.h>

#include "arm/mmu.h"
#include "hw/int.h"
#include "hw/hid.h"
#include "hw/lcd.h"
#include "hw/gx.h"
#include "hw/timer.h"

// MMU Translation Table (16KiB aligned)
static mmu_sect_t ALIGNV(1 << 14) _mmu_tt[4096];

// Default VRAM layout
const gx_framebuffers_t _default_framebuffers =
{
    {{VRAM_START, VRAM_START},
    {VRAM_START, VRAM_START},
    {VRAM_START, VRAM_START}}
};

vu32 *const irqvector_base = (vu32*)0x1FFFFFA0;
/*
 irqvector_base[0] is the instruction exectuted
 0xE51FF004 == 'ldr pc, [pc, #-4]'
*/

void main(void);
extern void XRQ_IRQ(void);

/*
 Low level initialization
  - Resets all devices to a known good state (or as close as possible)
  - Sets up the MMU and the first level translation table
  - Clears VRAM and sets up some LCD/GPU parameters
  - Sets some CR/ACR bits
  - Enables interrupts
  - Branches to main code
  - Other 'fun' things
*/
void ll_init(void)
{
    u32 page;

    // Reset devices
    irq_reset();
    mmu_reset();
    hid_reset();
    timer_reset();

    // Install interrupt handler
    irqvector_base[0] = 0xE51FF004;
    irqvector_base[1] = (u32)&XRQ_IRQ;

    // MMU Translation Table Mapping
    // Boot ROM
    _mmu_tt[0x000] = mmu_make_desc(0x00000000, false, 0, MMU_NONCACHABLE, MMU_AP_RO_NA, false);

    // IO
    for (page = 0x10100000; page < 0x10500000; page += MMU_PAGE_SIZE)
        _mmu_tt[page >> 20] = mmu_make_desc(page, false, 0, MMU_DEVICE_NONSHARED, MMU_AP_RW_NA, true);

    // MPCore Private Memory Region
    _mmu_tt[0x17E] = mmu_make_desc(0x17E00000, false, 0, MMU_DEVICE_NONSHARED, MMU_AP_RW_NA, true);

    // VRAM
    for (page = 0x18000000; page < 0x18600000; page += MMU_PAGE_SIZE)
        _mmu_tt[page >> 20] = mmu_make_desc(page, false, 0, MMU_WRITETHRU_NOWRALLOC, MMU_AP_RW_NA, true);

    // AXI BUS Work RAM
    _mmu_tt[0x1FF] = mmu_make_desc(0x1FF00000, false, 0, MMU_WRITEBACK_WRALLOC, MMU_AP_RW_NA, false);

    // FCRAM
    for (page = 0x20000000; page < 0x28000000; page += MMU_PAGE_SIZE)
        _mmu_tt[page >> 20] = mmu_make_desc(page, false, 0, MMU_WRITEBACK_WRALLOC, MMU_AP_RW_NA, true);

    // BootROM
    _mmu_tt[0xFFF] = mmu_make_desc(0xFFF00000, false, 0, MMU_NONCACHABLE, MMU_AP_RO_NA, false);

    // Set up the TTBR and invalidate the entire TLB
    mmu_set_ttbr((u32*)_mmu_tt, 0x12);
    mmu_invalidate_tlb();

    // Enable return stack, branch prediction and instruction folding
    _write_ACR(0xF);

    // Enable MMU, caches, enable program flow prediction, disable subpages, enable unaligned access
    _write_CR(_read_CR() | 0xC01805);


    // Show nothing but black on the screens
    lcd_fill(0, 0, 0);

    // Half brightness
    lcd_set_brightness(0x80);

    // Clear used VRAM
    gx_psc_fill(VRAM_START, VRAM_SIZE, 0x00000000, PSC_FILL32);

    // Set up the framebuffer modes
    gx_set_framebuffer_mode(GL_RGB565);

    // Placeholder VRAM layout
    gx_set_framebuffers(&_default_framebuffers);

    lcd_stop_fill();

    _enable_irqs();
    main();
    return;
}
