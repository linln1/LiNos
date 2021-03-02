#include <../libs/stdio.h>
#include <../libs/stddef.h>
#include <../libs/string.h>
#include <../driver/console.h>
#include <../debug/kdebug.h>
#include <trap.h>
#include <../driver/intr.h>
#include <../driver/clock.h>
#include <../mm/pmm.h>

void kern_init(void) __attribute__((noreturn));
static void print_cur_status(void) __attribute__((noreturn));
static void switch2usermode(void) __attribute__((always_inline));
static void switch2kernmode(void) __attribute__((always_inline));

void kern_init(void){
    extern char etext[], edata[], end[], kern_init[];
    memset(edata, 0, end - edata);

    console_init();

    const char *message = "[LiNos] is loading... ";
    cprintf("%s\n", message);

    cprintf("Special kernel symbols:\n");
    cprintf("  entry  0x%08x (phys)\n", kern_init);
    cprintf("  etext  0x%08x (phys)\n", etext);
    cprintf("  edata  0x%08x (phys)\n", edata);
    cprintf("  end    0x%08x (phys)\n", end);
    cprintf("Kernel executable memory footprint: %dKB\n", (end - kern_init + 1023)/1024);

    pmm_init();

    pic_init();
    idt_init();

    clock_init();
    intr_enable();

    while(1);
}

static void print_cur_status(void){
    static int round = 0;
    uint16_t reg1, reg2, reg3, reg4;
    asm volatile(
            "mov %%cs, %0;"
            "mov %%ds, %1;"
            "mov %%es, %2;"
            "mov %%ss, %3;"
            : "=m"(reg1), "=m"(reg2), "=m"(reg3), "=m"(reg4)
    );
    cprintf("%d: @ring %d\n", round, reg1 & 3);
    cprintf("%d:  cs = %x\n", round, reg1);
    cprintf("%d:  ds = %x\n", round, reg2);
    cprintf("%d:  es = %x\n", round, reg3);
    cprintf("%d:  ss = %x\n", round, reg4);
    round ++;
}

static void switch2usermode(void) {
    //TODO, 实现按ctrl + shift + 1 切换到内核态
	asm volatile (
	    "sub $0x8, %%esp \n"
	    "int %0 \n"
	    "movl %%ebp, %%esp"
	    : 
	    : "i"(T_SWITCH_TOU)
	);
}

static void switch2kernmode(void) {
    //TODO, 实现按ctrl + shift + 2 切换到用户态
	asm volatile (
	    "int %0 \n"
	    "movl %%ebp, %%esp \n"
	    : 
	    : "i"(T_SWITCH_TOK)
	);
}