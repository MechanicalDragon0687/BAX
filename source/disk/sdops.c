#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <common.h>
#include <disk/fatfs/ff.h>
#include <disk/sdmmc/sdmmc.h>
#include <io/printf.h>

bool file_exists(const char *path)
{
    FIL f;
    FRESULT ret = f_open(&f, path, FA_READ);
    if (!ret) f_close(&f);
    return (!ret);
}

size_t file_size(const char *path)
{
    FIL f;
    size_t ret = 0;
    FRESULT fz = f_open(&f, path, FA_READ);
    if (!fz)
    {
        ret = f_size(&f);
        f_close(&f);
    }
    return ret;
}

u32 file_read_offset(void *dest, const char *path, const u32 offset, const u32 max_size)
{
    FIL f;
    u32 ret = 0;
    FRESULT fz = f_open(&f, path, FA_READ);
    if (!fz)
    {
        f_lseek(&f, offset);
        fz = f_read(&f, dest, max_size, (size_t*)&ret);
        f_close(&f);
    }
    return ret;
}

u32 file_read(void *dest, const char *path, const u32 max_size)
{
    return file_read_offset(dest, path, 0, max_size);
}

u32 find_files(const char *base_path, const char *pattern, const u32 max, char out[][_MAX_LFN + 1])
{
    u32 ctr = 0, base_path_len;
    FRESULT fret;
    DIR dirent;
    FILINFO fno;

    base_path_len = strlen(base_path);
    if (base_path_len > _MAX_LFN) return 0;

    fret = f_findfirst(&dirent, &fno, base_path, pattern);
    while (fret == FR_OK && fno.fname[0] && ctr < max)
    {
        if (!(fno.fattrib & AM_DIR) && out[ctr])
        {
            memset(out[ctr], 0, _MAX_LFN + 1);

            strcpy(out[ctr], base_path);
            strncat(out[ctr], "/", _MAX_LFN + 1);
            strncat(out[ctr], fno.fname, _MAX_LFN + 1);

            ctr++;
        }
        fret = f_findnext(&dirent, &fno);
    }

    f_closedir(&dirent);
    return ctr;
}
