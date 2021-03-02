#include <stddef.h>
#include <error.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <asm/io.h>
#include <asm/memory.h>
#include <asm/segment.h>
#include <asm/system.h>


static const char* error_type[MAXERROR + 1] = {
    [0]                     NULL,
    [ERROR_UNSPECIFIED]     "[ERROR unspecified]",
    [ERROR_BAD_PROC]        "[BAD PROCESS]",
    [ERROR_INVAL]           "[INVALID PARAMETER]",
    [ERROR_NO_MEM]          "[OOM]",
    [ERROR_NO_FREE_PROC]    "[Out Of Process]",
    [ERROR_FAULT]           "[SEGMENTATION FAULT]",
};

static void printnum(void (*putch)(int, void*), void *putdat,
    uint64_t num, unsigned base, int width, int padc) {
    uint64_t result = num;
    unsigned mod = do_div(result, base);

    // first recursively print all preceding (more significant) digits
    if (num >= base) {
        printnum(putch, putdat, result, base, width - 1, padc);
    } else {
        // print any needed pad characters before first digit
        while (-- width > 0)
            putch(padc, putdat);
    }
    // then print this (the least significant) digit
    putch("0123456789abcdef"[mod], putdat);
}

static uint64_t getuint(va_list *ap, int lflag) {
    if (lflag >= 2) {
        return va_arg(*ap, uint64_t);
    }
    else if (lflag) {
        return va_arg(*ap, unsigned long);
    }
    else {
        return va_arg(*ap, uint32_t);
    }
}

static int64_t getint(va_list *ap, int lflag) {
    if (lflag >= 2) {
        return va_arg(*ap, int64_t);
    }
    else if (lflag) {
        return va_arg(*ap, long);
    }
    else {
        return va_arg(*ap, int32_t);
    }
}

void printfmt(void (*putch)(int, void*), void *putdat, const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    vprintfmt(putch, putdat, fmt, ap);
    va_end(ap);
}

void vprintfmt(void (*putch)(int, void*), void *putdat, const char *fmt, va_list ap) {
    register const char *p;
    register int ch, err;
    unsigned long long num;
    int base, width, precision, lflag, altflag;

    while (1) {
        while ((ch = *(unsigned char *)fmt ++) != '%') {
            if (ch == '\0') {
                return;
            }
            putch(ch, putdat);
        }

        // Process a %-escape sequence
        char padc = ' ';
        width = precision = -1;
        lflag = altflag = 0;

    reswitch:
        switch (ch = *(unsigned char *)fmt ++) {

        // flag to pad on the right
        case '-':
            padc = '-';
            goto reswitch;

        // flag to pad with 0's instead of spaces
        case '0':
            padc = '0';
            goto reswitch;

        // width field
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            for (precision = 0; ; ++ fmt) {
                precision = precision * 10 + ch - '0';
                ch = *fmt;
                if (ch < '0' || ch > '9') {
                    break;
                }
            }
            goto process_precision;

        case '*':
            precision = va_arg(ap, int);
            goto process_precision;

        case '.':
            if (width < 0)
                width = 0;
            goto reswitch;

        case '#':
            altflag = 1;
            goto reswitch;

        process_precision:
            if (width < 0)
                width = precision, precision = -1;
            goto reswitch;

        // long flag (doubled for long long)
        case 'l':
            lflag ++;
            goto reswitch;

        // character
        case 'c':
            putch(va_arg(ap, int), putdat);
            break;

        // error message
        case 'e':
            err = va_arg(ap, int);
            if (err < 0) {
                err = -err;
            }
            if (err > MAXERROR || (p = error_type[err]) == NULL) {
                printfmt(putch, putdat, "error %d", err);
            }
            else {
                printfmt(putch, putdat, "%s", p);
            }
            break;

        // string
        case 's':
            if ((p = va_arg(ap, char *)) == NULL) {
                p = "(null)";
            }
            if (width > 0 && padc != '-') {
                for (width -= strnlen(p, precision); width > 0; width --) {
                    putch(padc, putdat);
                }
            }
            for (; (ch = *p ++) != '\0' && (precision < 0 || -- precision >= 0); width --) {
                if (altflag && (ch < ' ' || ch > '~')) {
                    putch('?', putdat);
                }
                else {
                    putch(ch, putdat);
                }
            }
            for (; width > 0; width --) {
                putch(' ', putdat);
            }
            break;

        // (signed) decimal
        case 'd':
            num = getint(&ap, lflag);
            if ((long long)num < 0) {
                putch('-', putdat);
                num = -(long long)num;
            }
            base = 10;
            goto number;

        // unsigned decimal
        case 'u':
            num = getuint(&ap, lflag);
            base = 10;
            goto number;

        // (unsigned) octal
        case 'o':
            num = getuint(&ap, lflag);
            base = 8;
            goto number;

        // pointer
        case 'p':
            putch('0', putdat);
            putch('x', putdat);
            num = (unsigned long long)(uintptr_t)va_arg(ap, void *);
            base = 16;
            goto number;

        // (unsigned) hexadecimal
        case 'x':
            num = getuint(&ap, lflag);
            base = 16;
        number:
            printnum(putch, putdat, num, base, width, padc);
            break;

        // escaped '%' character
        case '%':
            putch(ch, putdat);
            break;

        // unrecognized escape sequence - just print it literally
        default:
            putch('%', putdat);
            for (fmt --; fmt[-1] != '%'; fmt --)
                /* do nothing */;
            break;
        }
    }
}


/* sprintbuf 用来保存缓冲区的足够信息*/
struct sprintbuf {
    char *buf;            // address pointer points to the first unused memory
    char *ebuf;            // points the end of the buffer
    int cnt;            // the number of characters that have been placed in this buffer
};

/* 在缓冲区中打印一个字符 */
static void sprintputch(int ch, struct sprintbuf *b) {
    b->cnt ++;
    if (b->buf < b->ebuf) {
        *b->buf ++ = ch;
    }
}

/* snprintf 将一个string格式化放到缓冲区中*/
int snprintf(char *str, size_t size, const char *fmt, ...) {
    va_list ap;
    int cnt;
    va_start(ap, fmt);
    cnt = vsnprintf(str, size, fmt, ap);
    va_end(ap);
    return cnt;
}

/* vsnprintf 将一个string格式化放到缓冲区中 传递参数中有一个va_list*/
int vsnprintf(char *str, size_t size, const char *fmt, va_list ap) {
    struct sprintbuf b = {str, str + size - 1, 0};
    if (str == NULL || b.buf > b.ebuf) {
        return -ERROR_INVAL;
    }
    // print the string to the buffer
    vprintfmt((void*)sprintputch, &b, fmt, ap);
    // null terminate the buffer
    *b.buf = '\0';
    return b.cnt;
}

