#include "diskio.h"

#include <cache.h>
#include <pxicmd.h>

#define SECTOR_SIZE  (512)

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE pdrv       /* Physical drive nmuber to identify the drive */
)
{
    return 0;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE pdrv               /* Physical drive nmuber to identify the drive */
)
{
    return PXICMD_Send(PXICMD_ARM9_SDMMC_INIT, NULL, 0);
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE pdrv,      /* Physical drive nmuber to identify the drive */
    BYTE *buff,     /* Data buffer to store read data */
    DWORD sector,   /* Start sector in LBA */
    UINT count      /* Number of sectors to read */
)
{
    int ret;
    CACHE_WbDCRange(buff, SECTOR_SIZE);
    CACHE_WbDCRange(buff + (count * SECTOR_SIZE), SECTOR_SIZE);

    ret = PXICMD_Send(PXICMD_ARM9_SD_READSECTORS, (u32[]){sector, count, (u32)buff}, 3);

    CACHE_InvDCRange(buff, count * SECTOR_SIZE);
    return ret;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
    BYTE pdrv,      /* Physical drive nmuber (0..) */
    BYTE cmd,       /* Control code */
    void *buff      /* Buffer to send/receive control data */
)
{
    return RES_PARERR;
}

