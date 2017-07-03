#include <common.h>
#include <limits.h>
#include <stdarg.h>

static const char char_dict[] = "0123456789ABCDEF";

/* padlen_log[i] = log(1<<32,i+2) */
static const int padlen_log[] = {-1, -1, 32, 21, 16, 14, 13, 12, 11, 11, 10, 10, 9, 9, 9, 9, 8};

static inline bool misnumber(char c)
{
    if (c < '0' || c > '9') {
        return false;
    } else {
        return true;
    }
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
void mitos(uint32_t n, int base, const int padlen, char *buf)
{
    int length, i;

    if (base < 2 || base > 16) {
        base = 16;
    }

    for (i = 0; i < min(padlen_log[base], padlen); i++) {
        buf[i] = '0';
    }

    i = 0;
    do {
        buf[i++] = char_dict[n % base];
        n /= base;
    } while(n != 0);

    length = max(padlen, i);
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
