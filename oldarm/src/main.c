#include <common.h>

#include <cpu.h>
#include <anim.h>
#include <firm.h>
#include <gfx.h>
#include <pxi.h>
#include <ff/ff.h>
#include <arm/irq.h>
#include <arm/bootfirm.h>

void pxi_sync_handler(u32 xrq)
{
    u8 msg = pxi_recv_rem();
    FIL firm;
    size_t br;

    switch(msg) {
    case PXI_CMD_FIRMBOOT:
        if (f_open(&firm, BASE_PATH FIRM_SUBPATH, FA_READ) == FR_OK) {
            f_read(&firm, FIRM_BUFFER, FIRM_MAXSIZE, &br);
            pxi_send_rem(PXI_CMD_FIRMBOOT);
            pxi_sync();
            if (firm_check(FIRM_BUFFER, FIRM_MAXSIZE)) {
                BootFirm(FIRM_BUFFER, SDMC_PATH FIRM_SUBPATH);
            }
        }
        //poweroff();
        break;
    default:
        break;
    }
    return;
}

int main(int argc, char *argv[])
{
    FATFS fs_main;
    FIL baxfil;
    FRESULT res;
    size_t br;
    anim_header *hdr;

    if (f_mount(&fs_main, "", 1) != FR_OK) {
        //poweroff();
    }

    pxi_reset();
    irq_reset();

    pxi_enable_irq();
    irq_register(IRQ_PXI_SYNC, pxi_sync_handler);

    pxi_sync();
    while(1);

    res = f_open(&baxfil, BASE_PATH ANIM_SUBPATH, FA_READ);
    if (res == FR_OK && f_size(&baxfil) <= ANIM_MAXSIZE) {
        f_read(&baxfil, ANIM_BUFFER, ANIM_MAXSIZE, &br);
        if (f_size(&baxfil) == br && anim_header_valid(ANIM_BUFFER, br)) {
            cpu_WritebackDCRange((u32)ANIM_BUFFER, (u32)ANIM_BUFFER + ANIM_MAXSIZE);

            pxi_putw((u32)ANIM_BUFFER);
            pxi_putw(br);
            pxi_send_rem(PXI_CMD_PLAYANIM);
            pxi_sync();
        }
    }

    f_close(&baxfil);

    while(1)
        wfi();
}
