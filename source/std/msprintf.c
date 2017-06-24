#include <common.h>
#include <limits.h>
#include <stdarg.h>
#include <string.h>

static const char char_dict[] = "0123456789ABCDEF";

/* padlen_log[i] = log(1<<32,i+2) */
static const char padlen_log[] = {32, 21, 16, 14, 13, 12, 11, 11, 10, 10, 9, 9, 9, 9, 8};

/*
 this only works with 32 bit integers
 no need (currently) for 64 bit ints
*/
void itos(int n, int base, const int padlen, char *buf)
{
    int length, i;
    unsigned int a;

    if (base < 2 || base > 16)
        return;

    if (base == 10 && n < 0) {
        /* special case for decimal base */
        a = (uint32_t)(-n);
        *(buf++) = '-';
    } else {
        a = (uint32_t)n;
    }

    for (i = 0; i < min(padlen_log[base - 2], padlen); i++)
        buf[i] = '0';

    i = 0;
    do {
        buf[i++] = char_dict[a % base];
        a /= base;
    } while(a != 0);

    length = max(padlen, i);
    for (i = 0; i < length/2; i++)
        SWAP(buf[i], buf[length - i - 1]);

    buf[length] = '\0';
    return;
}

char *msprintf(char *out, const char *fmt, ...)
{
    va_list va;
    char *s, *o, bufstr[(sizeof(int) * CHAR_BIT) + 2];
    const char *p;
    unsigned int i;

    va_start(va, fmt);

    o = out;
    for (p = fmt; *p != '\0'; p++) {
        if (*p != '%') {
            *(o++) = *p;
            continue;
        }

        switch(*(++p)) {
        case 'c':
            i = va_arg(va, int);
            *(o++) = i;
            break;
        case 's':
            s = va_arg(va, char *);
            strcpy(o, s);
            break;
        case 'X':
            i = va_arg(va, int);
            itos(i, 16, 8, bufstr);
            strcpy(o, bufstr);
            break;
        case 'd':
            i = va_arg(va, int);
            itos(i, 10, 0, bufstr);
            strcpy(o, bufstr);
            break;
        case '%':
            *(o++) = '%';
            break;
        }

        while(*o != '\0')
            o++;
    }

    va_end(va);
    return out;
}
