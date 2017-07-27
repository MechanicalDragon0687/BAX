#include <common.h>
#include <limits.h>
#include <stdarg.h>

const char char_dict[] = "0123456789ABCDEF";

/* padlen_lut[i] = log(1<<32,i) */
const int padlen_lut[] = {-1,-1,32,21,16,14,13,12,11,11,10,10,9,9,9,9,8};

static inline bool misnumber(char c)
{
    return RANGE(c, '0', '9');
}

uint32_t matoi(const char *s)
{
    uint32_t ret;
    char c;
    ret = 0;
    while(s && (c = *(s++))) {
        if (!misnumber(c)) {
            break;
        }
        ret *= 10;
        ret += c - '0';
    }
    return ret;
}

/* only 32 bit ints pls */
void mitos(uint32_t n, int base, int padlen, char *buf)
{
    int length, i;

    base = CLAMP(base, 2, 16);

    for (i = 0; i < MIN(padlen_lut[base], padlen); i++) {
        buf[i] = '0';
    }

    i = 0;
    do {
        buf[i++] = char_dict[n % base];
        n /= base;
    } while(n != 0);

    length = MAX(padlen, i);
    for (i = 0; i < length/2; i++) {
        SWAP(buf[i], buf[length - i - 1]);
    }

    buf[length] = '\0';
    return;
}

char *msprintf(char *out, const char *fmt, ...)
{
    va_list va;
    char *s, *o, bufstr[(sizeof(int) * CHAR_BIT) + 2];
    const char *p;
    int i, pad;

    va_start(va, fmt);

    o = out;
    for (p = fmt; *p != '\0'; p++) {
        pad = 0;

        if (*p != '%') {
            *(o++) = *p;
            continue;
        }

        if (misnumber(*(++p))) {
            pad = matoi(p);
            while(misnumber(*(++p)));
        }

        switch(*p) {
        case 'c':
            i = va_arg(va, int);
            *(o++) = i;
            break;
        case 's':
            s = va_arg(va, char*);
            strcpy(o, s);
            break;
        case 'X':
            i = va_arg(va, int);
            mitos(i, 16, pad, bufstr);
            strcpy(o, bufstr);
            break;
        case 'd':
            i = va_arg(va, int);
            mitos(i, 10, pad, bufstr);
            strcpy(o, bufstr);
            break;
        case '%':
            *(o++) = '%';
            break;
        }

        while(*o != '\0') {
            o++;
        }
    }

    va_end(va);
    return out;
}
