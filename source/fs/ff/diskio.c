/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include <hw/sdmmc.h>
#include <common.h>


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv UNUSED		/* Physical drive number to identify the drive */
)
{
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv UNUSED		/* Physical drive number to identify the drive */
)
{
	return sdmmc_sdcard_init();
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv UNUSED, /* Physical drive number to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	return sdmmc_sdcard_readsectors(sector, count, (uint32_t*)buff);
}


#ifndef _FS_READONLY
/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv UNUSED, /* Physical drive number to identify the drive */
	const BYTE *buff UNUSED,	/* Data to be written */
	DWORD sector UNUSED,		/* Start sector in LBA */
	UINT count UNUSED			/* Number of sectors to write */
)
{
	return RES_PARERR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv UNUSED,		/* Physical drive number (0..) */
	BYTE cmd UNUSED,	     	/* Control code */
	void *buff UNUSED		/* Buffer to send/receive control data */
)
{
	return RES_PARERR;
}
