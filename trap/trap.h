#ifndef __TRAP_TRAP_H__
#define __TRAP_TRAP_H__

#include <libs/ctype.h>

/* 陷入号 */

/* 处理器-软中断号*/
#define T_DIVIDE                0   // 除0
#define T_DEBUG                 1   // debug异常
#define T_NMI                   2   // non-maskable interrupt
#define T_BRKPT                 3   // 断点软中断
#define T_OFLOW                 4   // 溢出
#define T_BOUND                 5   // 边界检查
#define T_ILLOP                 6   // 非法运算符
#define T_DEVICE                7   // 设备不可用
#define T_DBLFLT                8   // double fault
#define T_COPROC                9   // 保留
#define T_TSS                   10  // 非法任务段切换
#define T_SEGNP                 11  // segment not present
#define T_STACK                 12  // 栈异常
#define T_GPFLT                 13  // 保护错
#define T_PGFLT                 14  // 页错误
#define T_RES                   15  // 保留
#define T_FPERR                 16  // 浮点数错误
#define T_ALIGN                 17  // 对齐检查
#define T_MCHK                  18  // 机器检查
#define T_SIMDERR               19  // SIMD 浮点错误

#define T_SYSCALL               0x80    //SYSCALL, ONLY FOR THIS PROJ

/* 硬件IRQ数 */
#define IRQ_OFFSET              32  // IRQ 0

#define IRQ_TIMER               0
#define IRQ_KBD                 1
#define IRQ_COM1                4
#define IRQ_IDE1                14
#define IRQ_IDE2                15
#define IRQ_ERROR               19
#define IRQ_SPURIOUS            31

/* 软中断，切换权限*/
#define T_SWITCH_TO_USER                120    // user/kernel switch
#define T_SWITCH_TO_KERN                121    // user/kernel switch

/* 寄存器 */
struct push_regs{
    uint32_t reg_edi;
    uint32_t reg_esi;
    uint32_t reg_ebp;
    uint32_t reg_oesp;
    uint32_t reg_ebx;
    uint32_t reg_edx;
    uint32_t reg_ecx;
    uint32_t reg_eax;
};

/* 陷入帧 
 * 4个数据段寄存器，四个padding用来扩展，一个软中断号
 * 指针数据结构8字节，不需要扩展，寄存器需要扩展
 * 错误号，目前指令地址寄存器cs:eip, eflags, 特权级转换的时候还要ss, esp
*/
struct trapframe{
    struct push_regs tf_regs;
    uint16_t tf_gs;
    uint16_t tf_padding0;
    uint16_t tf_fs;
    uint16_t tf_padding1;
    uint16_t tf_es;
    uint16_t tf_padding2;
    uint16_t tf_ds;
    uint16_t tf_padding3;
    uint32_t tf_trapno;

    uint32_t tf_err;
    uptr_t tf_eip;
    uint16_t tf_cs;
    uint16_t tf_padding4;
    uint32_t tf_eflags;

    /* 特权级转换的时候，需要保存ss:esp */
    uptr_t tf_esp;
    uint16_t tf_ss;
    uint16_t tf_padding5;
} __attribute__((packed));

void idt_init(void);
void print_trapframe(struct trapframe *tf);
void print_regs(struct push_regs *regs);
bool trap_in_kernel(struct trapframe *tf);


#endif 