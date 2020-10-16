/*
 *  linux/init/main.c
 *
 *  (C) 1991  Linus Torvalds
 *  (C) 2020  modified and decorated by linln1
 */

#define __LIBRARY__
#include <unistd.h>
#include <time.h>

/*
 * we need this inline - forking from kernel space will result
 * in NO COPY ON WRITE (!!!), until an execve is executed. This
 * is no problem, but for the stack. This is handled by not letting
 * main() use the stack at all after fork(). Thus, no function
 * calls - which means inline code for fork too, as otherwise we
 * would use the stack upon exit from 'fork()'.
 *
 * Actually only pause and fork are needed inline, so that there
 * won't be any messing with the stack from main(), but we define
 * some others too.
 */
//_syscall 0表示无参数,1表示1个参数
static inline _syscall0(int,fork)	// fork进程,0表示无参数,1表示1个参数
static inline _syscall0(int,pause)	//系统调用,暂停进程的执行，直到接收到一个信号
static inline _syscall1(int,setup,void *,BIOS)	//系统调用，用于linux初始化
static inline _syscall0(int,sync)		//更新文件系统

#include <linux/tty.h>
#include <linux/sched.h>
#include <linux/head.h>
#include <asm/system.h>
#include <asm/io.h>

#include <stddef.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#include <linux/fs.h>

#include <string.h>

static char printbuf[1024];

extern char *strcpy();
extern int vsprintf();
extern void init(void);
extern void blk_dev_init(void);
extern void chr_dev_init(void);
extern void hd_init(void);
extern void floppy_init(void);
extern void mem_init(long start, long end);
extern long rd_init(long mem_start, int length);
extern long kernel_mktime(struct tm * tm);

static int sprintf(char * str, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsprintf(str, fmt, args);
	va_end(args);
	return i;
}

/*
 * This is set up by the setup-routine at boot-time
 */
#define EXT_MEM_K (*(unsigned short *)0x90002)
#define CON_ROWS ((*(unsigned short *)0x9000e) & 0xff)
#define CON_COLS (((*(unsigned short *)0x9000e) & 0xff00) >> 8)
#define DRIVE_INFO (*(struct drive_info *)0x90080)
#define ORIG_ROOT_DEV (*(unsigned short *)0x901FC)
#define ORIG_SWAP_DEV (*(unsigned short *)0x901FA)

/*
 * Yeah, yeah, it's ugly, but I cannot find how to do this correctly
 * and this seems to work. I anybody has more info on the real-time
 * clock I'd be interested. Most of this was trial and error, and some
 * bios-listing reading. Urghh.
 */

#define CMOS_READ(addr) ({ \
outb_p(0x80|addr,0x70); \
inb_p(0x71); \
})

#define BCD_TO_BIN(val) ((val)=((val)&15) + ((val)>>4)*10)
//读取1970-1-1 0:0 到开机时间的值, 如果time.tm_sec != CMOS_READ(0)，就会重新读取时间，这样做可以把读出来的时间误差控制在一秒以内 
static void time_init(void)
{
	struct tm time;

	do {
		time.tm_sec = CMOS_READ(0);
		time.tm_min = CMOS_READ(2);
		time.tm_hour = CMOS_READ(4);
		time.tm_mday = CMOS_READ(7);
		time.tm_mon = CMOS_READ(8);
		time.tm_year = CMOS_READ(9);
	} while (time.tm_sec != CMOS_READ(0));
	BCD_TO_BIN(time.tm_sec);
	BCD_TO_BIN(time.tm_min);
	BCD_TO_BIN(time.tm_hour);
	BCD_TO_BIN(time.tm_mday);
	BCD_TO_BIN(time.tm_mon);
	BCD_TO_BIN(time.tm_year);
	time.tm_mon--;
	startup_time = kernel_mktime(&time);
}

//机器具有的物理内从容量，缓冲区的末端地址，主存的开始地址，一般高速缓冲的结束地址就是主存的开始地址
static long memory_end = 0;	//物理内存容量
static long buffer_memory_end = 0;	//高速缓冲区末端地址
static long main_memory_start = 0;	//主存分页开始的得知
static char term[32];	

static char * argv_rc[] = { "/bin/sh", NULL };	//shell的执行参数
static char * envp_rc[] = { "HOME=/", NULL ,NULL };	//shell的执行环境

static char * argv[] = { "-/bin/sh",NULL };	//shell的登陆命令参数
static char * envp[] = { "HOME=/usr/root", NULL, NULL };	//shell的登录环境

struct drive_info { char dummy[32]; } drive_info; //用来存储硬盘参数表的信息

void main(void)		/* This really IS void, no error here. */
{			/* The startup routine assumes (well, ...) this */
/*
 * Interrupts are still disabled. Do necessary setups, then
 * enable them
 */
 	ROOT_DEV = ORIG_ROOT_DEV;			//fs/super.c 
 	SWAP_DEV = ORIG_SWAP_DEV;			//交换区
	sprintf(term, "TERM=con%dx%d", CON_COLS, CON_ROWS);
	envp[1] = term;	
	envp_rc[1] = term;
 	drive_info = DRIVE_INFO;
	memory_end = (1<<20) + (EXT_MEM_K<<10);
	memory_end &= 0xfffff000;	//主存的结束地址
	if (memory_end > 16*1024*1024)	//如果主存结束地址>16MB
		memory_end = 16*1024*1024;	//就把主存大小按16MB来计算
	if (memory_end > 12*1024*1024) 	//如果主存的结束地址>12MB
		buffer_memory_end = 4*1024*1024;	//就把缓冲区的大小设置为4MB
	else if (memory_end > 6*1024*1024)	//如果主存的结束地址>6MB
		buffer_memory_end = 2*1024*1024;	//就把缓冲区的大小设置为2MB
	else
		buffer_memory_end = 1*1024*1024;	//最后如果小于6MB, 把缓冲区的大小设置成1MB
	main_memory_start = buffer_memory_end;	//主存的开始地址就是缓冲buffer的结束地址
#ifdef RAMDISK		//如果定义了虚拟盘符
	main_memory_start += rd_init(main_memory_start, RAMDISK*1024);
#endif//主存的开始地址就要再加上虚拟盘的大小


	//内核初始化
	{
		mem_init(main_memory_start,memory_end);
		trap_init();
		blk_dev_init();
		chr_dev_init();
		tty_init();
		time_init();
		sched_init();
		buffer_init(buffer_memory_end);
		hd_init();
		floppy_init();
		sti();
	}

	move_to_user_mode();	//切换到用户模式
	if (!fork()) {		/* we count on this going ok */如果是子进程，就要在子进程中调用init函数
		init();
	}
/*
 *   NOTE!!   For any other task 'pause()' would mean we have to get a
 * signal to awaken, but task0 is the sole exception (see 'schedule()')
 * as task 0 gets activated at every idle moment (when no other tasks
 * can run). For task0 'pause()' just means we go check if some other
 * task can run, and if not we return here.
 */
// 除了任务0之外，其余所有的任务中调用pause()函数都是意味着我们需要唤醒一个信号，才能返回到就绪状态
// 任务0再任何空闲的时间段里都会被激活，所以对于任务0，pause 意味着我们返回来查看是否有其他任务可以运行，如果没有的话，就一直for:pause()
	for(;;)
		__asm__("int $0x80"::"a" (__NR_pause):"ax");
}

static int printf(const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	write(1,printbuf,i=vsprintf(printbuf, fmt, args));
	va_end(args);
	return i;
}

void init(void)
{
	int pid,i;

	setup((void *) &drive_info);
	(void) open("/dev/tty1",O_RDWR,0);
	(void) dup(0);
	(void) dup(0);
	printf("%d buffers = %d bytes buffer space\n\r",NR_BUFFERS,
		NR_BUFFERS*BLOCK_SIZE);
	printf("Free mem: %d bytes\n\r",memory_end-main_memory_start);
	if (!(pid=fork())) {
		close(0);
		if (open("/etc/rc",O_RDONLY,0))
			_exit(1);
		execve("/bin/sh",argv_rc,envp_rc);
		_exit(2);
	}
	if (pid>0)
		while (pid != wait(&i))
			/* nothing */;
	while (1) {
		if ((pid=fork())<0) {
			printf("Fork failed in init\r\n");
			continue;
		}
		if (!pid) {
			close(0);close(1);close(2);	
			setsid();
			(void) open("/dev/tty1",O_RDWR,0);	
			(void) dup(0);
			(void) dup(0);
			_exit(execve("/bin/sh",argv,envp));
		}
		while (1)//等待子进程结束
			if (pid == wait(&i))
				break;
		printf("\n\rchild %d died with code %04x\n\r",pid,i);
		sync();	//同步
	}
	_exit(0);	/* NOTE! _exit, not exit() */
}