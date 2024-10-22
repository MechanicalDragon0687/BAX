#ifndef FS_H
#define FS_H

#include <types.h>
#include "lib/ff/ff.h"

#define FS_MAXPATH (FF_MAX_LFN + 1)
#define FS_MAXSRCH (512)

typedef FATFS FS_Mount;

typedef struct {
    FIL ff;
    char *path;
} FS_File;

typedef struct {
    DIR fd;
    char *path;
    char **srch;
    size_t srchn;
} FS_Dir;


void FS_Init(void);
void FS_Stop(void);

bool     FS_FileExists(const char *path);
FS_File *FS_FileOpen(const char *path);
void     FS_FileClose(FS_File *f);
char    *FS_FilePath(FS_File *f);
void     FS_FileRead(FS_File *f, void *b, size_t l);
size_t   FS_FileSize(const FS_File *f);
size_t   FS_FileTell(const FS_File *f);
void     FS_FileSeek(FS_File *f, size_t p);


FS_Dir *FS_DirOpen(const char *path);
void    FS_DirClose(FS_Dir *d);
char   *FS_DirPath(FS_Dir *d);
void    FS_DirSearch(FS_Dir *d, const char *ext, size_t n);
size_t  FS_DirSearchCount(const FS_Dir *d);
char   *FS_DirSearchResult(const FS_Dir *d, size_t n);

#endif // FS_H
