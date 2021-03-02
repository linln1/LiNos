#include <stddef.h>
#include <stdio.h>
#include <console.h>

/* HIGH level console I/O */

static void cputch(int c, int *cnt){
    cons_putc(c);
    (*cnt)++;
}

/* 将string以fmt格式输出到stdout */
int vcprintf(const char *fmt, va_list ap){
    int cnt = 0;
    vprintfmt((void*)cputch, &cnt, fmt, ap);
    return cnt;
}

/* 将string以fmt格式输出，返回值是写入到stdout的个数 */
int cprintf(const char *fmt, ...) {
    va_list ap;
    int cnt;
    va_start(ap, fmt);
    cnt = vcprintf(fmt, ap);
    va_end(ap);
    return cnt;
}

/* 写一个单一的字符到stdout */
void cputchar(int c){
    cons_putc(c);
}

/* 将字符串指针写入到stdout 并且加一个换行符*/
int cputs(const char *str) {
    int cnt = 0;
    char c;
    while ((c = *str ++) != '\0') {
        cputch(c, &cnt);
    }
    cputch('\n', &cnt);
    return cnt;
}

/* 从stdin读一个非零字符*/
int getchar(void) {
    int c;
    while ((c = cons_getc()) == 0)
        /* do nothing */;
    return c;
}
