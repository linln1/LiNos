#ifndef __KERN_TRAP_TRAP_H__
#define __KERN_TRAP_TRAP_H__

#include <defs.h>

/* Trap Numbers */

/* Processor-defined: */
#define T_DIVIDE                0   // 除零
#define T_DEBUG                 1   // 调试异常
#define T_NMI                   2   // non-maskable interrupt
#define T_BRKPT                 3   // 断点
#define T_OFLOW                 4   // 溢出
#define T_BOUND                 5   // 边界检查
#define T_ILLOP                 6   // 非法操作码
#define T_DEVICE                7   // 不可用设备
#define T_DBLFLT                8   // 浮点错误
// #define T_COPROC             9   // reserved (not used since 486)
#define T_TSS                   10  // TSS转换段
#define T_SEGNP                 11  // 段不能访问
#define T_STACK                 12  // 栈异常
#define T_GPFLT                 13  // 通用保护出错
#define T_PGFLT                 14  // 页错误
// #define T_RES                15  // 保留
#define T_FPERR                 16  // 浮点异常
#define T_ALIGN                 17  // 对其检查
#define T_MCHK                  18  // 机器检查
#define T_SIMDERR               19  // SIMD floating point error

/* Hardware IRQ numbers. We receive these as (IRQ_OFFSET + IRQ_xx) */
#define IRQ_OFFSET              32  // IRQ 0 corresponds to int IRQ_OFFSET

#define IRQ_TIMER               0
#define IRQ_KBD                 1
#define IRQ_COM1                4
#define IRQ_IDE1                14
#define IRQ_IDE2                15
#define IRQ_ERROR               19
#define IRQ_SPURIOUS            31

/* *
 * These are arbitrarily chosen, but with care not to overlap
 * processor defined exceptions or interrupt vectors.
 * */
#define T_SWITCH_TOU                120    // user/kernel switch
#define T_SWITCH_TOK                121    // user/kernel switch

/* registers as pushed by pushal */
struct pushregs {
    uint32_t reg_edi;
    uint32_t reg_esi;
    uint32_t reg_ebp;
    uint32_t reg_oesp;          /* Useless */
    uint32_t reg_ebx;
    uint32_t reg_edx;
    uint32_t reg_ecx;
    uint32_t reg_eax;
};

struct trapframe {
    struct pushregs tf_regs;
    uint16_t tf_gs;
    uint16_t tf_padding0;
    uint16_t tf_fs;
    uint16_t tf_padding1;
    uint16_t tf_es;
    uint16_t tf_padding2;
    uint16_t tf_ds;
    uint16_t tf_padding3;
    uint32_t tf_trapno;
    /* below here defined by x86 hardware */
    uint32_t tf_err;
    uintptr_t tf_eip;
    uint16_t tf_cs;
    uint16_t tf_padding4;
    uint32_t tf_eflags;
    /* below here only when crossing rings, such as from user to kernel */
    uintptr_t tf_esp;
    uint16_t tf_ss;
    uint16_t tf_padding5;
} __attribute__((packed));

void idt_init(void);
void print_trapframe(struct trapframe *tf);
void print_regs(struct pushregs *regs);
bool trap_in_kernel(struct trapframe *tf);

#endif /* !__KERN_TRAP_TRAP_H__ */

