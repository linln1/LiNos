#include <defs.h>
#include <list.h>
#include <proc.h>
#include <assert.h>
#include <fcfs_sched.h>

static void
FCFS_init(struct run_queue *rq) {
    list_init(&(rq->run_list));  //初始化进程队列 
    rq->proc_num = 0;
}

static void
FCFS_enqueue(struct run_queue *rq, struct proc_struct *proc) {   //入队 
    assert(list_empty(&(proc->run_link)));
    list_add_before(&(rq->run_list), &(proc->run_link));
    if (proc->time_slice == 0 || proc->time_slice > rq->max_time_slice) {
        proc->time_slice = rq->max_time_slice;   //分配时间片 
    }
    proc->rq = rq;
    rq->proc_num ++;
}

static void
FCFS_dequeue(struct run_queue *rq, struct proc_struct *proc) {  //出队 
    assert(!list_empty(&(proc->run_link)) && proc->rq == rq);
    list_del_init(&(proc->run_link));
    rq->proc_num --;
}

static struct proc_struct *
FCFS_pick_next(struct run_queue *rq) {
    list_entry_t *le = list_next(&(rq->run_list));
    if (le != &(rq->run_list)) {
        return le2proc(le, run_link);
    }
    return NULL;
}

static void
FCFS_proc_tick(struct run_queue *rq, struct proc_struct *proc) {
    // if (proc->time_slice > 0) {
    //     proc->time_slice --;
    // }
    // if (proc->time_slice == 0) {
    //     proc->need_resched = 1;
    // }
}

static int
FCFS_user_check(void){
	return 0;
}

struct sched_class fcfs_sched_class = {
    .name = "FCFS_scheduler",
    .init = FCFS_init,
    .enqueue = FCFS_enqueue,
    .dequeue = FCFS_dequeue,
    .pick_next = FCFS_pick_next,
    .proc_tick = FCFS_proc_tick,
    .user_check = FCFS_user_check,
};

