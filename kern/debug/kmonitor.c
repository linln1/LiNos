#include <stdio.h>
#include <string.h>
#include <mmu.h>
#include <trap.h>
#include <kmonitor.h>
#include <kdebug.h>

/* *
 * 主要是⽤于实现内核下的⼀些基本监控命令 。定义了一个数
 * 据结构commands(命令行内核监视)器，用于控制内核，
 * 向其发送命令以及和系统进行交互。
 * */

struct command {
    const char *name;
    const char *desc;
    //返回-1强制monitor退出
    int(*func)(int argc, char **argv, struct trapframe *tf);
};

static struct command commands[] = {
    {"help", "Display this list of commands.", mon_help},
    {"kerninfo", "Display information about the kernel.", mon_kerninfo},
    {"backtrace", "Print backtrace of stack frame.", mon_backtrace},
};

/* return if kernel is panic, in kern/debug/panic.c */
bool is_kernel_panic(void);

#define NCOMMANDS (sizeof(commands)/sizeof(struct command))

/***** Kernel monitor command interpreter *****/

#define MAXARGS         16
#define WHITESPACE      " \t\n\r"

/* parse：将命令缓冲区中的信息buffer解析为空格分隔的参数 */
static int
parse(char *buf, char **argv) {
    int argc = 0;
    while (1) {
        // find global whitespace
        while (*buf != '\0' && strchr(WHITESPACE, *buf) != NULL) {
            *buf ++ = '\0';
        }
        if (*buf == '\0') {
            break;
        }

        // 保存并扫描下一个参数
        if (argc == MAXARGS - 1) {
            cprintf("Too many arguments (max %d).\n", MAXARGS);
        }
        argv[argc ++] = buf;
        while (*buf != '\0' && strchr(WHITESPACE, *buf) == NULL) {
            buf ++;
        }
    }
    return argc;
}

/* *
 * runcmd: 解析输入的字符串，将其拆分成不同的参数，
 * 然后查找并调用一些相关的命令
 * */
static int
runcmd(char *buf, struct trapframe *tf) {
    char *argv[MAXARGS];
    int argc = parse(buf, argv);
    if (argc == 0) {
        return 0;
    }
    int i;
    for (i = 0; i < NCOMMANDS; i ++) {
        if (strcmp(commands[i].name, argv[0]) == 0) {
            return commands[i].func(argc - 1, argv + 1, tf);
        }
    }
    cprintf("Unknown command '%s'\n", argv[0]);
    return 0;
}

/***** Implementations of basic kernel monitor commands *****/

void
kmonitor(struct trapframe *tf) {
    cprintf("Welcome to the kernel debug monitor!!\n");
    cprintf("Type 'help' for a list of commands.\n");

    if (tf != NULL) {
        print_trapframe(tf);
    }

    char *buf;
    while (1) {
        if ((buf = readline("K> ")) != NULL) {
            if (runcmd(buf, tf) < 0) {
                break;
            }
        }
    }
}

/* mon_help - print the information about mon_* functions */
int
mon_help(int argc, char **argv, struct trapframe *tf) {
    int i;
    for (i = 0; i < NCOMMANDS; i ++) {
        cprintf("%s - %s\n", commands[i].name, commands[i].desc);
    }
    return 0;
}

/* *
 * mon_kerninfo：调用kern/debug/kdebug.c中的print_kerninfo来打印内核的内存占用情况。
 * */
int
mon_kerninfo(int argc, char **argv, struct trapframe *tf) {
    print_kerninfo();
    return 0;
}


//mon_backtrace：调用kern/debug/kdebug.c中的print_stackframe打印堆栈的回溯信息
int
mon_backtrace(int argc, char **argv, struct trapframe *tf) {
    print_stackframe();
    return 0;
}

