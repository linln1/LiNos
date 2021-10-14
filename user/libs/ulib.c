#include <defs.h>
#include <syscall.h>
#include <stdio.h>
#include <ulib.h>
#include <stat.h>
#include <string.h>
#include <lock.h>

static lock_t fork_lock = INIT_LOCK;

void
lock_fork(void) {
    lock(&fork_lock);
}

void
unlock_fork(void) {
    unlock(&fork_lock);
}

void
exit(int error_code) {
    sys_exit(error_code);
    cprintf("BUG: exit failed.\n");
    while (1);
}

int
fork(void) {
    return sys_fork();
}

int
wait(void) {
    return sys_wait(0, NULL);
}

int
waitpid(int pid, int *store) {
    return sys_wait(pid, store);
}

void
yield(void) {
    sys_yield();
}

int
kill(int pid) {
    return sys_kill(pid);
}

int
getpid(void) {
    return sys_getpid();
}

//print_pgdir - print the PDT&PT
void
print_pgdir(void) {
    sys_pgdir();
}

void
lab6_set_priority(uint32_t priority)
{
    sys_lab6_set_priority(priority);
}

int
sleep(unsigned int time) {
    return sys_sleep(time);
}

unsigned int
gettime_msec(void) {
    return (unsigned int)sys_gettime();
}

int
__exec(const char *name, const char **argv) {
    int argc = 0;
    while (argv[argc] != NULL) {
        argc ++;
    }
    return sys_exec(name, argc, argv);
}

int malloc(int n){
    return sys_malloc(n);
}

void getrunqueue(void){
    sys_queue();
}

void
listProc(void){
    sys_listProc();
} 

void
current_vm(void){
    sys_current_vm();
}

void
mem(void){
    sys_mem();
}

void
pmmcheck(void){
     sys_pmm_check();
}

void
swapcheck(void){
     sys_swap_check();
}

void
schedcheck(void){
     sys_sched_check();
}

void 
change_swap(unsigned int mode){
    sys_change_swap(mode);
}

void
change_sched(unsigned int mode){
    sys_change_sched(mode);
}

void stride_queue(void){
    sys_stride_queue();
}

void
listwait(void){
    sys_list_wait();
}

