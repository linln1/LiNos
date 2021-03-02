#include <libs/ctype.h>
#include <asm/io.h>
#include <asm/memory.h>
#include <asm/segment.h>
#include <asm/system.h>
#include <mm/pmm.h>
#include <mm/memlayout.h>
#include <mm/mmu.h>

static struct taskstate ts = {0};

/* *
 * Global Descriptor Table:
 *
 * 内核段和代码段是一样的 (除了 DPL). 为了导入ss寄存器，CPL必须等于DPL
 * 我们必须为用户态和内核态备份这些段，它们定义如下
 *   - 0x0 :  unused (always faults -- for trapping NULL far pointers)
 *   - 0x8 :  kernel code segment
 *   - 0x10:  kernel data segment
 *   - 0x18:  user code segment
 *   - 0x20:  user data segment
 *   - 0x28:  defined for tss, initialized in gdt_init
 * */
static struct segdesc gdt[] = {
    SEG_NULL,
    [SEG_KTEXT] = SEG(STA_X | STA_R , 0x0, 0xFFFFFFFF, DPL_KERNEL),
    [SEG_KDATA] = SEG(STA_W, 0x0, 0xFFFFFFFF, DPL_KERNEL),
    [SEG_UTEXT] = SEG(STA_X | STA_R, 0x0, 0xFFFFFFFF, DPL_USER),
    [SEG_UDATA] = SEG(STA_W, 0x0, 0xFFFFFFFF, DPL_USER),
    [SEG_TSS] = SEG_NULL,
};

static struct pseudodesc gdt_pd = {
    sizeof(gdt) - 1, (uint32_t)gdt
};

/* 装载全局描述符表寄存器并且为内核态重置数据、代码段寄存器 */
static inline void lgdt(struct pseudodesc *pd){
    asm volatile("lgdt (%0)" :: "r" (pd));
    asm volatile("movw %%ax, %%gs" :: "a" (USER_DS));
    asm volatile("movw %%ax, %%fs" :: "a" (USER_DS));
    asm volatile("movw %%ax, %%es" :: "a" (KERNEL_DS));
    asm volatile("movw %%ax, %%ds" :: "a" (KERNEL_DS));
    asm volatile("movw %%ax, %%ss" :: "a" (KERNEL_DS));
    //reload cs
    asm volatile("ljmp %0, $1f\n 1:\n" :: "i" (KERNEL_CS));
}

/* 临时内核栈 */
uint8_t kern_stack[1024];

static void gdt_init(){
    //设置TSS，当我们从用户态切换到内核态，我们能得到正确的栈。
    ts.ts_esp0 = (uint32_t)&kern_stack + sizeof(kern_stack);
    ts.ts_ss0 = KERNEL_DS;

    //初始化gdt的TSS域
    gdt[SEG_TSS] = SEG16(STS_T32A, (uint32_t)&ts, sizeof(ts), DPL_KERNEL);
    gdt[SEG_TSS].sd_s = 0;

    //重新装载所有段寄存器
    lgdt(&gdt_pd);
    //装载TSS
    ltr(GD_TSS);
}

/* 初始化物理内存 */
void pmm_init(){
    gdt_init();
}


