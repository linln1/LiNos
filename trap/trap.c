#include <libs/ctype.h>
#include <mm/mmu.h>
#include <mm/memlayout.h>
#include <driver/clock.h>
#include <trap/trap.h>
#include <asm/io.h>
#include <asm/memory.h>
#include <asm/segment.h>
#include <asm/system.h>
#include <debug/assert.h>
#include <driver/console.h>
#include <debug/kdebug.h>

#define TICKNUM 100

static void print_ticks(){
    cprintf("%d ticks\n", TICKNUM);
#ifdef DEBUG_GRADE
    cprintf("End of Test.\n");
    panic("EOT: kernel seems ok.");
#endif
}

/* idt 运行时必须被建立应为移动的函数地址不能够被再重定位记录中表示 */
static struct gatedesc idt[256] = {{0}};

static struct pseudodesc idt_pd = {
    sizeof(idt) - 1, (uptr_t)idt;
};

void idt_init(void){

}



