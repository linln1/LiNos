#ifndef _IO_H
#define _IO_H

#include <../ctype.h>

#define do_div(n, base) ({                                        \
    unsigned long __upper, __low, __high, __mod, __base;        \
    __base = (base);                                            \
    asm("" : "=a" (__low), "=d" (__high) : "A" (n));            \
    __upper = __high;                                            \
    if (__high != 0) {                                            \
        __upper = __high % __base;                                \
        __high = __high / __base;                                \
    }                                                            \
    asm("divl %2" : "=a" (__low), "=d" (__mod)                    \
        : "rm" (__base), "0" (__low), "1" (__upper));            \
    asm("" : "=A" (n) : "a" (__low), "d" (__high));                \
    __mod;                                                        \
 })

static inline uint8_t inb(uint16_t port) __attribute__((always_inline));
static inline void insl(uint32_t port, void* addr, int cnt) __attribute__((always_inline));
static inline void outb(uint16_t port, uint8_t data) __attribute__((always_inline));
static inline void outw(uint16_t port, uint16_t data) __attribute__((always_inline));
static inline uint32_t read_ebp(void) __attribute__((always_inline));

/* Pseudo-descriptors used for LGDT, LLDT(not used) and LIDT instructions. */
struct pseudodesc {
    uint16_t pd_lim;        // Limit
    uint32_t pd_base;        // Base address
} __attribute__ ((packed));

/* 从port端口edx读出数据保存到eax */
static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    asm volatile ("inb %1 %0" : "=a" (data) : "d" (port));
    return data;
}

/* */
static inline void insl(uint32_t port, void *addr, int cnt) {
    asm volatile (
            "cld;"
            "repne; insl;"
            : "=D" (addr), "=c" (cnt)
            : "d" (port), "0" (addr), "1" (cnt)
            : "memory", "cc");
}

/* 将一个字节的数据输出到port号端口 */
static inline void outb(uint16_t port, uint8_t data) {
    asm volatile ("outb %0, %1" :: "a" (data), "d" (port));
}

/* 将一个字的数据输出到port号端口 */
static inline void outw(uint16_t port, uint16_t data) {
    asm volatile ("outw %0, %1" :: "a" (data), "d" (port));
}

/* 读取ebp寄存器中的值 */
static inline uint32_t read_ebp(void) {
    uint32_t ebp;
    asm volatile ("movl %%ebp, %0" : "=r" (ebp));
    return ebp;
}

#endif /* !IO_H */