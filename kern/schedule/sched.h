#ifndef __KERN_SCHEDULE_SCHED_H__
#define __KERN_SCHEDULE_SCHED_H__

#include <defs.h>
#include <list.h>
#include <skew_heap.h>

struct proc_struct;

typedef struct {
    unsigned int expires;
    struct proc_struct *proc;
    list_entry_t timer_link;
} timer_t;

#define le2timer(le, member)            \
to_struct((le), timer_t, member)

static inline timer_t *
timer_init(timer_t *timer, struct proc_struct *proc, int expires) {
    timer->expires = expires;
    timer->proc = proc;
    list_init(&(timer->timer_link));
    return timer;
}

struct run_queue;

/**
 * 进程调度类是从Linux系统借鉴的，这种实现提升了内核调度器的拓展性
 * 这个类封装了进程的调度策略
 */
struct sched_class {
    // 调度器名
    const char *name;
    // 初始化就绪等待队列
    void (*init)(struct run_queue *rq);
    // 让进程PCB入队，需要给队列加锁
    void (*enqueue)(struct run_queue *rq, struct proc_struct *proc);
    // 让进程PCB出队，需要给队列加锁
    void (*dequeue)(struct run_queue *rq, struct proc_struct *proc);
    // 选择下一个运行的进程
    struct proc_struct *(*pick_next)(struct run_queue *rq);
    // 处理事件片
    void (*proc_tick)(struct run_queue *rq, struct proc_struct *proc);
    // 用户可以调用此函数来对调度器算法进行测试
    int (*user_check)(void);
};

struct run_queue {
    list_entry_t run_list;
    unsigned int proc_num;
    int max_time_slice;
    // For LAB6 ONLY
    skew_heap_entry_t *lab6_run_pool;
};


extern const struct sched_class *sched_class;
void sched_init(void);
void wakeup_proc(struct proc_struct *proc);
void schedule(void);
void add_timer(timer_t *timer);
void del_timer(timer_t *timer);
void run_timer_list(void);

#endif /* !__KERN_SCHEDULE_SCHED_H__ */

