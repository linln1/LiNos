#ifndef __USER_LIBS_SYSCALL_H__
#define __USER_LIBS_SYSCALL_H__

int sys_exit(int error_code);
int sys_fork(void);
int sys_wait(int pid, int *store);
int sys_exec(const char *name, int argc, const char **argv);
int sys_yield(void);
int sys_kill(int pid);
int sys_getpid(void);
int sys_putc(int c);
int sys_pgdir(void);
int sys_sleep(unsigned int time);
size_t sys_gettime(void);
int sys_malloc(int n);
void sys_pmm_check(void);
void sys_swap_check(void);
void sys_sched_check(void);

struct stat;
struct dirent;

int sys_open(const char *path, uint32_t open_flags);
int sys_delete(const char *path, uint32_t type);
int sys_close(int fd);
int sys_read(int fd, void *base, size_t len);
int sys_write(int fd, void *base, size_t len);
int sys_seek(int fd, off_t pos, int whence);
int sys_fstat(int fd, struct stat *stat);
int sys_fsync(int fd);
int sys_getcwd(char *buffer, size_t len);
int sys_mkdir(const char *path);
int sys_getdirentry(int fd, struct dirent *dirent);
int sys_dup(int fd1, int fd2);
void sys_lab6_set_priority(uint32_t priority); //only for lab6
void sys_queue();
void sys_listProc();
void sys_current_vm(void);
void sys_mem(void);
int sys_chcwd(const char *path);
int sys_rename(const char *oldName, const char *newName);
void sys_change_swap(unsigned int mode);
void sys_change_sched(unsigned int mode);
void sys_stride_queue(void);
void sys_list_wait(void);

#endif /* !__USER_LIBS_SYSCALL_H__ */

