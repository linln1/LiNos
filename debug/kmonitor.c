#include <libs/stdio.h>
#include <libs/string.h>
#include <trap.h>
#include <kmonitor.h>
#include <kdebug.h>


/* 一个简单的shell 内核监视器用来控制内核并且交互式地探索系统 */
struct command{
    const char* name;
    const char* desc;
    int(*func)(int argc, int **argv, struct trapframe *tf);
};


static struct command shell[] = {
    {"help", "Display this list of commands.", mon_help},
    {"kerninfo", "Display information about the kernel.", mon_kerninfo},
    {"backtrace", "Print backtrace of stack frame.", mon_backtrace},
};

#define NCOMMANDS (sizeof(shell)/sizeof(struct command))


/* shell 解释器 */
#define MAXARGS         16                  //shell 命令最大参数个数
#define WHITESPACE      " \t\n\r"           //空白字符


/* parser */
static int parse(char *buf, char **argv){
    int argc = 0;
    while(1){
        //找到空白键,分割指令
        while(*buf != 0 && strchr(WHITESPACE, *buf) != NULL){
            *buf++ = '\0';
        }
        if(*buf == '\0'){
            break;
        }

        if(argc == MAXARGS - 1){
            cprintf("too many args, should less than %d .\n", MAXARGS);
        }
        argv[argc++] = buf;
        while(*buf != '\0' && strchr(WHITESPACE, *buf) == NULL){
            buf++;
        }
    }
    return argc;
}

/* split command into separated args and lookup and invoke related commands*/
static int exec_cmd(char* buf, struct trapframe *tf){
    
}

void kmonitor(struct trapframe *tf){
    cprintf("Welcome to the kernel debug monitor!\n");
    cprintf("Type 'help' for a list of commands.\n");

    if(tf != NULL){
        print_stackframe(tf);
    }

    char *buf;
    while(1){
        if((buf = readline("K> ")) != NULL){
            if(exec_cmd(buf, tf) < 0){
                break;
            }
        }
    }
}

int mon_help(int argc, char **argv, struct trapframe *tf){
    for(int i = 0 ; i < NCOMMANDS ; i++){
        cprintf("%s - %s\n", shell[i].name, shell[i].desc);
    }
    return 0;
}

int mon_kern_info(int argc, char **argv, struct trapframe *tf){
    print_kerninfo();
    return 0;
}

int mon_backtrace(int argc, char **argv, struct trapframe *tf){
    print_stackframe();
    return 0;
}




