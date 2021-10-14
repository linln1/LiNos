#include <defs.h>
#include <list.h>
#include <proc.h>
#include <assert.h>
#include <prior_sched.h>

#define USE_SKEW_HEAP 1

#define BIG_STRIDE    0x7FFFFFFF /* ??? */

/* The compare function for two skew_heap_node_t's and the
 * corresponding procs*/
static int
proc_prior_comp_f(void *a, void *b)
{
     struct proc_struct *p = le2proc(a, lab6_run_pool);
     struct proc_struct *q = le2proc(b, lab6_run_pool);
     int32_t c = p->priority - q->priority;

     if (c > 0) return 1;
     else if (c == 0) return 0;
     else return -1;
}

 
  // priority_initʹ����ȷ�ķ����ʼ�����ж���rq
  //��Ա������������
  
  //-run_list����ʼ����ӦΪ���б���
  //-lab6_run_pool��NULL
  //-proc_num��0
  //-max_time_slice���˴�����Ҫ���ñ������ɵ��÷����䡣
  
  //�й��б��ṹ�����̣���μ� / libs / list.h��
   
static void
prior_init(struct run_queue *rq) {
     list_init(&(rq->run_list));
     rq->lab6_run_pool = NULL;
     rq->proc_num = 0;
}

/*
  * Prior_enqueue������``proc''�������ж���
  *``rq''�� �ó���Ӧ��֤/��ʼ����س�Ա
  *Ϊ``proc''��Ȼ��``lab6_run_pool''�ڵ����
  *���У���Ϊ����������ʹ�����ȼ����У��� �ó���Ӧ��
  *��``rq''�ṹ����Ԫ���ڡ�
  *
  * proc-> time_slice��ʾʱ��Ƭ�ķ���
  *���̣�Ӧ��������Ϊrq-> max_time_slice��
  *
  *�й����ȼ������̣���μ�/ libs / skew_heap.h
  *���нṹ��
  */
static void
prior_enqueue(struct run_queue *rq, struct proc_struct *proc) {
#if USE_SKEW_HEAP
     rq->lab6_run_pool =
          skew_heap_insert(rq->lab6_run_pool, &(proc->lab6_run_pool), proc_prior_comp_f);
#else
     assert(list_empty(&(proc->run_link)));
     list_add_before(&(rq->run_list), &(proc->run_link));
#endif
     if (proc->time_slice == 0 || proc->time_slice > rq->max_time_slice) {
          proc->time_slice = rq->max_time_slice;
     }
     proc->rq = rq;
     rq->proc_num ++;
}

/*
  * Prior_dequeue�����ж�����ɾ������``proc''
  *``rq''���ò�������skew_heap_remove���
  *������ ��סҪ����``rq''�ṹ��
  *
  *�й����ȼ������̣���μ�proj13.1 / libs / skew_heap.h
  *���нṹ��
  */
static void
prior_dequeue(struct run_queue *rq, struct proc_struct *proc) {

#if USE_SKEW_HEAP
     rq->lab6_run_pool =
          skew_heap_remove(rq->lab6_run_pool, &(proc->lab6_run_pool), proc_prior_comp_f);
#else
     assert(!list_empty(&(proc->run_link)) && proc->rq == rq);
     list_del_init(&(proc->run_link));
#endif
     rq->proc_num --;
}
/*
  * priority_pick_next��``���ж���''��ѡ��Ԫ�أ���ʹ��
  *�������Сֵ����������Ӧ�Ĺ���
  *ָ�롣 ����ָ�뽫�ɺ�le2proc���㣬
  *�йض��壬��μ�proj13.1 / kern / process / proc.h�� �������NULL
  *������û�н��̡�
  *
  *ѡ��һ��proc�ṹʱ�����סҪ����ǰһ��
  * proc�����ԡ� �����ȼ�+ = BIG_prior /���ȼ���
  *
  *�й����ȼ������̣���μ�/ libs / skew_heap.h
  *���нṹ��
  */
static struct proc_struct *
prior_pick_next(struct run_queue *rq) {

#if USE_SKEW_HEAP
     if (rq->lab6_run_pool == NULL) return NULL;
     struct proc_struct *p = le2proc(rq->lab6_run_pool, lab6_run_pool);
#else
     list_entry_t *le = list_next(&(rq->run_list));

     if (le == &rq->run_list)
          return NULL;
     
     struct proc_struct *p = le2proc(le, run_link);
     le = list_next(le);
     while (le != &rq->run_list)
     {
          struct proc_struct *q = le2proc(le, run_link);
          if ((int32_t)(p->lab6_prior - q->lab6_prior) > 0)
               p = q;
          le = list_next(le);
     }
#endif
     if(p->pid !=2 && p->pid!=1)
	{
		if(p->priority == 0){
			cprintf("\nproc %d is setting prior\n", p->pid);		
		}
		else
			cprintf("proc %d is running, [prior]=%u \n" ,p->pid, p->priority);
		//do_stride_queue(p->pid);
	}

     p->stride = p->priority;
     return p;
}


static int
prior_user_check(void){
	return 0;
}
/*
  * Prior_proc_tick�뵱ǰ���̵�tick�¼�һ��ʹ�á� ��
  *Ӧ��鵱ǰ���̵�ʱ��Ƭ�Ƿ�Ϊ
  *�þ�������proc�ṹ``proc''�� proc-> time_slice
  *��ʾ��ǰʣ���ʱ��Ƭ
  * ���̡� proc-> need_resched�ǽ��̵ı�־����
  *�л���
  */
static void
prior_proc_tick(struct run_queue *rq, struct proc_struct *proc) {
	
}

struct sched_class prior_sched_class = {
     .name = "prior_scheduler",
     .init = prior_init,
     .enqueue = prior_enqueue,
     .dequeue = prior_dequeue,
     .pick_next = prior_pick_next,
     .proc_tick = prior_proc_tick,
     .user_check = prior_user_check,
};

