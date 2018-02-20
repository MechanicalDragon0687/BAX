#include <common.h>

#include "arm/bug.h"
#include "lib/ff/ff.h"
#include "lib/fs/fs.h"



/* Mount operations */
static FS_Mount FSMain;
void FS_Init(void)
{
    int res = f_mount(&FSMain, "SDMC:", 1);
    if (res != FR_OK)
        BUG(BUGSTR("FS_INIT"), 1, BUGINT(res), 1);
}

void FS_Stop(void)
{
    f_mount(NULL, "SDMC:", 1);
}



/* File operations */
FS_File *FS_FileOpen(const char *path)
{
    size_t pathlen;
    FRESULT res;
    FS_File *ret;

    assert(path != NULL);

    pathlen = strlen(path);
    assert(pathlen < FS_MAXPATH);

    ret = malloc(sizeof(FS_File));
    assert(ret != NULL);

    ret->path = malloc(pathlen + 1);
    assert(ret->path != NULL);

    strcpy(ret->path, path);

    res = f_open(&ret->ff, path, FA_READ);
    if (res != FR_OK)
        BUG(BUGSTR("FS_FILEOPEN", path), 2, BUGINT(res), 1);

    return ret;
}

void FS_FileClose(FS_File *f)
{
    assert(f != NULL);
    f_close(&f->ff);
    free(f->path);
    free(f);
}

char *FS_FilePath(FS_File *f)
{
    assert(f != NULL);
    return f->path;
}

void FS_FileRead(FS_File *f, void *b, size_t l)
{
    size_t br;
    FRESULT res;
    assert(f != NULL);

    res = f_read(&f->ff, b, l, &br);
    if (res != FR_OK)
        BUG(BUGSTR("FS_FILEREAD", f->path), 2, BUGINT(l, br), 2);
}

size_t FS_FileSize(const FS_File *f)
{
    assert(f != NULL);
    return f_size(&f->ff);
}

size_t FS_FileGetPos(const FS_File *f)
{
    assert(f != NULL);
    return f_tell(&f->ff);
}

void FS_FileSetPos(FS_File *f, size_t p)
{
    size_t sz;
    assert(f != NULL);

    sz = FS_FileSize(f);

    if (p > sz)
        BUG(BUGSTR("FS_FILESETPOS", f->path), 2, BUGINT(sz, p), 2);

    f_lseek(&f->ff, p);
}



/* Directory operations */
FS_Dir *FS_DirOpen(const char *path)
{
    size_t pathlen;
    FRESULT res;
    FS_Dir *ret;

    assert(path != NULL);

    pathlen = strlen(path);
    assert(pathlen < FS_MAXPATH);

    ret = malloc(sizeof(FS_Dir));
    assert(ret != NULL);

    ret->path = malloc(pathlen + 1);
    assert(ret->path != NULL);

    strcpy(ret->path, path);

    res = f_opendir(&ret->fd, path);
    if (res != FR_OK)
        BUG(BUGSTR("FS_DIROPEN", path), 2, BUGINT(res), 1);

    ret->srch  = NULL;
    ret->srchn = 0;
    return ret;
}

static void FS_DirSearchClear(FS_Dir *d)
{
    assert(d != NULL);
    if (d->srchn != 0) {
        for (size_t i = 0; i < d->srchn; i++)
            free(d->srch[i]);
        free(d->srch);
    }
    d->srch  = NULL;
    d->srchn = 0;
}

void FS_DirClose(FS_Dir *d)
{
    assert(d != NULL);
    FS_DirSearchClear(d);
    free(d->path);
    free(d);
}

char *FS_DirPath(FS_Dir *d)
{
    assert(d != NULL);
    return d->path;
}

void FS_DirSearch(FS_Dir *d, const char *ext, size_t n)
{
    FRESULT res;
    FILINFO fno;
    size_t count = 0, pathlen, extlen, baselen;
    char *paths[n], *pathext, *basepath;

    assert(d != NULL);
    assert(ext != NULL);
    assert(n < FS_MAXSRCH);

    basepath = d->path;

    baselen = strlen(basepath);
    extlen  = strlen(ext);

    FS_DirSearchClear(d);

    do {
        res = f_readdir(&d->fd, &fno);
        if (res != FR_OK || fno.fname[0] == 0)
            break;

        if (fno.fattrib & AM_DIR)
            continue;

        pathlen = strlen(fno.fname);
        if (pathlen < extlen)
            continue;

        pathext = &fno.fname[pathlen - extlen];

        if (strcmp(pathext, ext) == 0) {
            pathlen = strlen(fno.fname);
            paths[count] = malloc(baselen + 1 + pathlen + 1);
            assert(paths[count] != NULL);

            strcpy(paths[count], basepath);
            strcpy(&paths[count][baselen + 1], fno.fname);
            paths[count][baselen] = '/';

            count++;
        }
    } while(res == FR_OK);

    if (count > 0) {
        d->srchn = count;
        d->srch  = malloc(sizeof(char*) * count);
        assert(d->srch != NULL);

        for (size_t i = 0; i < count; i++)
            d->srch[i] = paths[i];        
    }

    // Close and reopen the directory handle
    f_closedir(&d->fd);
    res = f_opendir(&d->fd, d->path);
    assert(res == FR_OK);
}

size_t FS_DirSearchCount(const FS_Dir *d)
{
    assert(d != NULL);
    return d->srchn;
}

char *FS_DirSearchResult(const FS_Dir *d, size_t n)
{
    assert(d != NULL);
    assert(n < d->srchn);
    return d->srch[n];
}
