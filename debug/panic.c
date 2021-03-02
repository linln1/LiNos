#include <../libs/ctype.h>
#include <../libs/stdio.h>
#include <../driver/intr.h>
#include <../libs/stdarg.h>
#include <kmonitor.h>

static bool is_panic = 0;

/**
 * __panic 在遇到不可解决的致命错误的时候被调用，它打印
 * "panic: 'message'"之后进入内核监视器
 */
void __panic(const char *file, int line, const char *fmt, ...){
    if(is_panic){
        goto panic_dead;
    }
    is_panic = 1;

    va_list ap;
    va_start(ap, fmt);
    cprintf("kernel panic at %s:%d:\n   ", file, line);
    vcprintf(fmt, ap);
    cprintf("\n");

    cprintf("stack trackback: \n");
    print_stackframe();

    va_end(ap);

panic_dead:
    intr_disable();
    while (1)
    {
        kmonitor(NULL);
    }
    
}

/* __warn */
void __warn(const char *file, int line, const char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    cprintf("kernel warning at %s:%d:\n     ", file, line);
    vcprintf(fmt, ap);
    cprintf("\n");
    va_end(ap);
}

bool is_kernel_panic(void){
    return is_panic;
}