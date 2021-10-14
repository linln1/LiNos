#include <defs.h>
#include <unistd.h>
#include <stdarg.h>
#include <syscall.h>
#include <stat.h>
#include <dirent.h>


#define MAX_ARGS            5

static inline int
syscall(int num, ...) {
    va_list ap;
    va_start(ap, num);
    uint32_t a[MAX_ARGS];
    int i, ret;
    for (i = 0; i < MAX_ARGS; i ++) {
        a[i] = va_arg(ap, uint32_t);
    }
    va_end(ap);

    asm volatile (
        "int %1;"
        : "=a" (ret)
        : "i" (T_SYSCALL),
          "a" (num),
          "d" (a[0]),
          "c" (a[1]),
          "b" (a[2]),
          "D" (a[3]),
          "S" (a[4])
        : "cc", "memory");
    return ret;
}

int
sys_exit(int error_code) {
    return syscall(SYS_exit, error_code);
}

int
sys_fork(void) {
    return syscall(SYS_fork);
}

int
sys_wait(int pid, int *store) {
    return syscall(SYS_wait, pid, store);
}

int
sys_yield(void) {
    return syscall(SYS_yield);
}

int
sys_kill(int pid) {
    return syscall(SYS_kill, pid);
}

int
sys_getpid(void) {
    return syscall(SYS_getpid);
}

int
sys_putc(int c) {
    return syscall(SYS_putc, c);
}

int
sys_pgdir(void) {
    return syscall(SYS_pgdir);
}

void
sys_lab6_set_priority(uint32_t priority)
{
    syscall(SYS_lab6_set_priority, priority);
}

int
sys_sleep(unsigned int time) {
    return syscall(SYS_sleep, time);
}

size_t
sys_gettime(void) {
    return syscall(SYS_gettime);
}

int
sys_exec(const char *name, int argc, const char **argv) {
    return syscall(SYS_exec, name, argc, argv);
}

int
sys_open(const char *path, uint32_t open_flags) {
    return syscall(SYS_open, path, open_flags);
}

int
sys_delete(const char *path, uint32_t type) {
	return syscall(SYS_delete, path, type);
}

int
sys_close(int fd) {
    return syscall(SYS_close, fd);
}

int
sys_read(int fd, void *base, size_t len) {
    return syscall(SYS_read, fd, base, len);
}

int
sys_write(int fd, void *base, size_t len) {
    return syscall(SYS_write, fd, base, len);
}

int
sys_seek(int fd, off_t pos, int whence) {
    return syscall(SYS_seek, fd, pos, whence);
}

int
sys_fstat(int fd, struct stat *stat) {
    return syscall(SYS_fstat, fd, stat);
}

int
sys_fsync(int fd) {
    return syscall(SYS_fsync, fd);
}

int
sys_getcwd(char *buffer, size_t len) {
    return syscall(SYS_getcwd, buffer, len);
}

int
sys_mkdir(const char *path) {
	return syscall(SYS_mkdir, path);
}

int
sys_getdirentry(int fd, struct dirent *dirent) {
    return syscall(SYS_getdirentry, fd, dirent);
}

int
sys_dup(int fd1, int fd2) {
    return syscall(SYS_dup, fd1, fd2);
}

int
sys_malloc(int n){
    return syscall(SYS_malloc, n);
}

void
sys_queue(void){
    syscall(SYS_queue);
}

void
sys_listProc(void){
    syscall(SYS_listProc);
}

void
sys_current_vm(void){
    syscall(SYS_current_vm);
}

void
sys_mem(void){
    syscall(SYS_mem);
}

int
sys_chcwd(const char *path) {
	return syscall(SYS_chcwd, path);
}

int
sys_rename(const char *oldName, const char *newName) {
	return syscall(SYS_rename, oldName, newName);
}

void sys_pmm_check(void){
	syscall(SYS_check_pmm);
}

void sys_swap_check(void){
	syscall(SYS_check_swap);
}

void sys_sched_check(void){
	syscall(SYS_check_sched);
}

void 
sys_change_swap(unsigned int mode){
    syscall(SYS_change_swap,mode);
}

void 
sys_change_sched(unsigned int mode){
    syscall(SYS_change_sched,mode);
}

void
sys_stride_queue(void){
    syscall(SYS_stride_queue);
}

void
sys_list_wait(void){
    syscall(SYS_list_wait);
}
