#include <list.h>
#include <sync.h>
#include <proc.h>
#include <sched.h>
#include <stdio.h>
#include <assert.h>
#include <stride_sched.h>
#include <fcfs_sched.h>
#include <rr_sched.h>
#include <prior_sched.h>

static list_entry_t timer_list;

const struct sched_class *sched_class;

static struct run_queue *rq;

static inline void
sched_class_enqueue(struct proc_struct *proc) {
    if (proc != idleproc) {
        sched_class->enqueue(rq, proc);
    }
}

static inline void
sched_class_dequeue(struct proc_struct *proc) {
    sched_class->dequeue(rq, proc);
}

static inline struct proc_struct *
sched_class_pick_next(void) {
    return sched_class->pick_next(rq);
}

static void
sched_class_proc_tick(struct proc_struct *proc) {
    if (proc != idleproc) {
        sched_class->proc_tick(rq, proc);
    }
    else {
        proc->need_resched = 1;
    }
}

static struct run_queue __rq;

void
sched_init(void) {
    list_init(&timer_list);

    sched_class = &rr_sched_class;

    rq = &__rq;
    rq->max_time_slice = 5;
    sched_class->init(rq);
    cprintf("\n5.调度器\n");
    cprintf("sched class: %s\n", sched_class->name);
}

void
wakeup_proc(struct proc_struct *proc) {
    assert(proc->state != PROC_ZOMBIE);
    bool intr_flag;
    local_intr_save(intr_flag);
    {
        if (proc->state != PROC_RUNNABLE) {
            proc->state = PROC_RUNNABLE;
            proc->wait_state = 0;
            if (proc != current) {
                sched_class_enqueue(proc);
            }
        }
        else {
            warn("wakeup runnable process.\n");
        }
    }
    local_intr_restore(intr_flag);
}

void
schedule(void) {
    bool intr_flag;
    struct proc_struct *next;
    local_intr_save(intr_flag);
    {
        current->need_resched = 0;
        if (current->state == PROC_RUNNABLE) {
            sched_class_enqueue(current);
        }
        if ((next = sched_class_pick_next()) != NULL) {
            sched_class_dequeue(next);
        }
        if (next == NULL) {
            next = idleproc;
        }
        next->runs ++;
        if (next != current) {
            proc_run(next);
        }
    }
    local_intr_restore(intr_flag);
}

void
add_timer(timer_t *timer) {
    bool intr_flag;
    local_intr_save(intr_flag);
    {
        assert(timer->expires > 0 && timer->proc != NULL);
        assert(list_empty(&(timer->timer_link)));
        list_entry_t *le = list_next(&timer_list);
        while (le != &timer_list) {
            timer_t *next = le2timer(le, timer_link);
            if (timer->expires < next->expires) {
                next->expires -= timer->expires;
                break;
            }
            timer->expires -= next->expires;
            le = list_next(le);
        }
        list_add_before(le, &(timer->timer_link));
    }
    local_intr_restore(intr_flag);
}

void
del_timer(timer_t *timer) {
    bool intr_flag;
    local_intr_save(intr_flag);
    {
        if (!list_empty(&(timer->timer_link))) {
            if (timer->expires != 0) {
                list_entry_t *le = list_next(&(timer->timer_link));
                if (le != &timer_list) {
                    timer_t *next = le2timer(le, timer_link);
                    next->expires += timer->expires;
                }
            }
            list_del_init(&(timer->timer_link));
        }
    }
    local_intr_restore(intr_flag);
}

void
run_timer_list(void) {
    bool intr_flag;
    local_intr_save(intr_flag);
    {
        list_entry_t *le = list_next(&timer_list);
        if (le != &timer_list) {
            timer_t *timer = le2timer(le, timer_link);
            assert(timer->expires != 0);
            timer->expires --;
            while (timer->expires == 0) {
                le = list_next(le);
                struct proc_struct *proc = timer->proc;
                if (proc->wait_state != 0) {
                    assert(proc->wait_state & WT_INTERRUPTED);
                }
                else {
                    warn("process %d's wait_state == 0.\n", proc->pid);
                }
                wakeup_proc(proc);
                del_timer(timer);
                if (le == &timer_list) {
                    break;
                }
                timer = le2timer(le, timer_link);
            }
        }
        sched_class_proc_tick(current);
    }
    local_intr_restore(intr_flag);
}

void
do_queue(){
    list_entry_t *le=list_next(&(rq->run_list));
    //cprintf("\t%d",rq->proc_num);
    while(le != &(rq->run_list))
    {
        struct proc_struct *proc=le2proc(le,run_link);
        cprintf("\t%d",proc->pid);
        le=list_next(le);
    }
}

void do_change_sched(unsigned int mode)
{
     //printf("mode:%u",mode);
     switch (mode)
     {
     case 1:
          sched_class =&stride_sched_class;
          break;
     case 2:
          sched_class =&fcfs_sched_class;
          break;
     case 3:
          sched_class =&rr_sched_class;
          break;
     case 4:
	  sched_class =&prior_sched_class;
	  break;
     default:
          cprintf("bad argument!\n");
          break;
     }
     cprintf("change sched: %s !\n",sched_class->name);
}

struct stride_entry{
     int pid;
     int stride;
     int priority;
};

int inorder(skew_heap_entry_t *a,struct stride_entry q[],int n){
     if(a==NULL)
          return n;
     struct proc_struct *p = le2proc(a, lab6_run_pool);
     //printf("%d[%d] ",p->pid,p->stride);
     q[n].stride=p->stride;
     q[n].pid=p->pid;
     q[n].priority=p->priority;
     n++;
     n=inorder(a->left,q,n);
     n=inorder(a->right,q,n);
     return n;


}

void sort(struct stride_entry q[],int n)
{
     int i,j;
     for(i=0;i<n;i++)
     {
          for(j=i+1;j<n;j++)
          {
               if(q[j].stride<q[i].stride)
               {
                    struct stride_entry temp;
                    temp=q[j];
                    q[j]=q[i];
                    q[i]=temp;
               }
          }
     }
}

static inline void skew_heap_trav(skew_heap_entry_t *a, int pid){
     struct stride_entry queue[100];
     int num=0;
     num=inorder(a,queue,num);
     sort(queue,num);
     int i;
//     cprintf("queue:");
     for(i=0;i<num;i++)
     {
	if(pid != queue[i].pid)
          cprintf("\t pid=%u priority=[%u] \n",queue[i].pid,queue[i].stride);
          //printf("%d ",queue[i].pid);
     }
     cprintf("\n-------------\n");
}

void do_stride_queue(int pid)
{
    skew_heap_trav(rq->lab6_run_pool, pid);
}
