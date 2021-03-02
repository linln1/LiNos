#ifndef __LIBS_STDIO_H__
#define __LIBS_STDIO_H__

#include <../libs/ctype.h>
#include <libs/stdarg.h>


/* kern/libs/stdio.c */
int getchar(void);
int cputs(const char* str);
void cputchar(int c);
int cprintf(const char* fmt, ...);
int vcprintf(const char* fmt, va_list ap);
int printf(char* fmt, ...);
void error(char *fmt, ...);

/* kern/libs/readline.c */
char* readline(const char* prompt);

/* libs/printfmt.c */
void printfmt(void (*putch)(int, void*), void* putdat, const char* fmt, ...);
void vprintfmt(void (*putch)(int, void*), void* putdat, const char* fmt, va_list ap);
int snprintf(char* str, size_t size, const char* fmt, ...);
int vsnprintf(char* str, size_t size, const char* fmt, va_list ap);

#endif /* !__LIBS_STDIO_H__ */