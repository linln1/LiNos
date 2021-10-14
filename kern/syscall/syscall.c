#include <defs.h>
#include <unistd.h>
#include <proc.h>
#include <syscall.h>
#include <trap.h>
#include <stdio.h>
#include <pmm.h>
#include <assert.h>
#include <clock.h>
#include <stat.h>
#include <dirent.h>
#include <sysfile.h>
#include <swap.h>
#include <sched.h>

static int
sys_exit(uint32_t arg[]) {
    int error_code = (int)arg[0];
    return do_exit(error_code);
}

static int
sys_fork(uint32_t arg[]) {
    struct trapframe *tf = current->tf;
    uintptr_t stack = tf->tf_esp;
    return do_fork(0, stack, tf);
}

static int
sys_wait(uint32_t arg[]) {
    int pid = (int)arg[0];
    int *store = (int *)arg[1];
    return do_wait(pid, store);
}

static int
sys_exec(uint32_t arg[]) {
    const char *name = (const char *)arg[0];
    int argc = (int)arg[1];
    const char **argv = (const char **)arg[2];
    return do_execve(name, argc, argv);
}

static int
sys_yield(uint32_t arg[]) {
    return do_yield();
}

static int
sys_kill(uint32_t arg[]) {
    int pid = (int)arg[0];
    return do_kill(pid);
}

static int
sys_getpid(uint32_t arg[]) {
    return current->pid;
}

static int
sys_putc(uint32_t arg[]) {
    int c = (int)arg[0];
    cputchar(c);
    return 0;
}

static int
sys_pgdir(uint32_t arg[]) {
    print_pgdir();
    return 0;
}

static uint32_t
sys_gettime(uint32_t arg[]) {
    return (int)ticks;
}
static uint32_t
sys_lab6_set_priority(uint32_t arg[])
{
    uint32_t priority = (uint32_t)arg[0];
    lab6_set_priority(priority);
    return 0;
}

static int
sys_sleep(uint32_t arg[]) {
    unsigned int time = (unsigned int)arg[0];
    return do_sleep(time);
}

static int
sys_open(uint32_t arg[]) {
    const char *path = (const char *)arg[0];
    uint32_t open_flags = (uint32_t)arg[1];
    return sysfile_open(path, open_flags);
}

static int
sys_delete(uint32_t arg[]) {
	const char *path = (const char *)arg[0];
	uint32_t type = (uint32_t)arg[1];
	return sysfile_delete(path, type);
}

static int
sys_close(uint32_t arg[]) {
    int fd = (int)arg[0];
    return sysfile_close(fd);
}

static int
sys_read(uint32_t arg[]) {
    int fd = (int)arg[0];
    void *base = (void *)arg[1];
    size_t len = (size_t)arg[2];
    return sysfile_read(fd, base, len);
}

static int
sys_write(uint32_t arg[]) {
    int fd = (int)arg[0];
    void *base = (void *)arg[1];
    size_t len = (size_t)arg[2];
    return sysfile_write(fd, base, len);
}

static int
sys_seek(uint32_t arg[]) {
    int fd = (int)arg[0];
    off_t pos = (off_t)arg[1];
    int whence = (int)arg[2];
    return sysfile_seek(fd, pos, whence);
}

static int
sys_fstat(uint32_t arg[]) {
    int fd = (int)arg[0];
    struct stat *stat = (struct stat *)arg[1];
    return sysfile_fstat(fd, stat);
}

static int
sys_fsync(uint32_t arg[]) {
    int fd = (int)arg[0];
    return sysfile_fsync(fd);
}

static int
sys_getcwd(uint32_t arg[]) {
    char *buf = (char *)arg[0];
    size_t len = (size_t)arg[1];
    return sysfile_getcwd(buf, len);
}

static int
sys_mkdir(uint32_t arg[]) {
	const char *path = (const char *)arg[0];
	return sysfile_mkdir(path);
}

static int
sys_getdirentry(uint32_t arg[]) {
    int fd = (int)arg[0];
    struct dirent *direntp = (struct dirent *)arg[1];
    return sysfile_getdirentry(fd, direntp);
}

static int
sys_dup(uint32_t arg[]) {
    int fd1 = (int)arg[0];
    int fd2 = (int)arg[1];
    return sysfile_dup(fd1, fd2);
}

static int
sys_malloc(uint32_t arg[]){
    int n = (int)arg[0];
    return do_malloc(n);
}

static void
sys_queue(void){
    do_queue();
}

static int 
sys_listProc(uint32_t arg[]){
    print_listProc();
    return 0;
}

static void
sys_current_vm(void){
    print_current_vm();
    return 0;
}

static void
sys_mem(void){
    do_mem();
    return 0;
}

static int
sys_chcwd(uint32_t arg[]) {
	const char *path = (const char *)arg[0];
	return sysfile_chdir(path);
}

static int
sys_rename(uint32_t arg[]) {
	const char *oldName = (const char *)arg[0];
	const char *newName = (const char *)arg[1];
	return sysfile_rename(oldName, newName);
}

static void
sys_check_pmm(uint32_t arg[]){
	pmm_manager->user_check();
	cprintf("check successed !\n");
}

static void
sys_check_swap(uint32_t arg[]){
    	sm->user_check();
	cprintf("check successed !\n");
}

static void
sys_check_sched(uint32_t arg[]){
	sched_class->user_check();
	cprintf("check successed !\n");
}

static int
sys_change_swap(uint32_t arg[]){
    do_change_swap((unsigned int)arg[0]);
    return 0;
}

static int 
sys_change_sched(uint32_t arg[]){
    do_change_sched((unsigned int)arg[0]);
    return 0;
}

static int
sys_stride_queue(void){
    do_stride_queue();
    return 0;
}

static void
sys_list_wait(void){
	do_list_wait();
}

static int (*syscalls[])(uint32_t arg[]) = {
    [SYS_exit]              sys_exit,
    [SYS_fork]              sys_fork,
    [SYS_wait]              sys_wait,
    [SYS_exec]              sys_exec,
    [SYS_yield]             sys_yield,
    [SYS_kill]              sys_kill,
    [SYS_getpid]            sys_getpid,
    [SYS_putc]              sys_putc,
    [SYS_pgdir]             sys_pgdir,
    [SYS_gettime]           sys_gettime,
    [SYS_lab6_set_priority] sys_lab6_set_priority,
    [SYS_sleep]             sys_sleep,
    [SYS_open]              sys_open,
	[SYS_delete]			sys_delete,
    [SYS_close]             sys_close,
    [SYS_read]              sys_read,
    [SYS_write]             sys_write,
    [SYS_seek]              sys_seek,
    [SYS_fstat]             sys_fstat,
    [SYS_fsync]             sys_fsync,
    [SYS_getcwd]            sys_getcwd,
	[SYS_mkdir]				sys_mkdir,
	[SYS_chcwd]				sys_chcwd,
    [SYS_getdirentry]       sys_getdirentry,
    [SYS_dup]               sys_dup,
    [SYS_malloc]	    sys_malloc,
    [SYS_mem]		    sys_mem,
    [SYS_queue]             sys_queue,
    [SYS_listProc]          sys_listProc,
    [SYS_current_vm]	    sys_current_vm,
	[SYS_rename]				sys_rename,
    [SYS_check_pmm]         sys_check_pmm,
    [SYS_check_swap]        sys_check_swap,
    [SYS_check_sched]       sys_check_sched,
    [SYS_change_swap]       sys_change_swap,
    [SYS_change_sched]      sys_change_sched,
    [SYS_stride_queue]      sys_stride_queue,
    [SYS_list_wait]	    sys_list_wait,
};

#define NUM_SYSCALLS        ((sizeof(syscalls)) / (sizeof(syscalls[0])))

void
syscall(void) {
    struct trapframe *tf = current->tf;
    uint32_t arg[5];
    int num = tf->tf_regs.reg_eax;
    if (num >= 0 && num < NUM_SYSCALLS) {
        if (syscalls[num] != NULL) {
            arg[0] = tf->tf_regs.reg_edx;
            arg[1] = tf->tf_regs.reg_ecx;
            arg[2] = tf->tf_regs.reg_ebx;
            arg[3] = tf->tf_regs.reg_edi;
            arg[4] = tf->tf_regs.reg_esi;
            tf->tf_regs.reg_eax = syscalls[num](arg);
            return ;
        }
    }
    print_trapframe(tf);
    panic("undefined syscall %d, pid = %d, name = %s.\n",
            num, current->pid, current->name);
}

