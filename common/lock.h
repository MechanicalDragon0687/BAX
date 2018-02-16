#ifndef LOCK_H
#define LOCK_H

#include <common.h>
#include <malloc.h>
#include <cpu.h>

// Ugly hack but newlib syscalls are messy af to work with
static inline void *LockMalloc(size_t s) {
    void *ret;
    CritStat cs = CPU_EnterCritical();
    ret = malloc(s);
    CPU_LeaveCritical(cs);
    return ret;
}

static inline void *LockMemalign(size_t a, size_t s) {
    void *ret;
    CritStat cs = CPU_EnterCritical();
    ret = memalign(a, s);
    CPU_LeaveCritical(cs);
    return ret;
}

static inline void LockFree(void *p) {
    CritStat cs = CPU_EnterCritical();
    free(p);
    CPU_LeaveCritical(cs);
}

#endif // LOCK_H
