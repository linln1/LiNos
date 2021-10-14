#include <defs.h>
#include <list.h>
#include <proc.h>
#include <assert.h>
#include <stride_sched.h>

#define USE_SKEW_HEAP 1

/*�ڴ˴�����BigStride����*/

#define BIG_STRIDE    0x7FFFFFFF /* ??? */

/*����skew_heap_node_t��
  *��Ӧ�Ľ���*/
static int
proc_stride_comp_f(void *a, void *b)
{
     struct proc_struct *p = le2proc(a, lab6_run_pool);
     struct proc_struct *q = le2proc(b, lab6_run_pool);
     int32_t c = p->stride - q->stride;
     if (c > 0) return 1;
     else if (c == 0) return 0;
     else return -1;
}

/*
  * stride_initʹ����ȷ�ķ����ʼ�����ж���rq
  *��Ա������������
  *
  *-run_list����ʼ����ӦΪ���б���
  *-lab6_run_pool��NULL
  *-proc_num��0
  *-max_time_slice���˴�����Ҫ���ñ������ɵ��÷����䡣
  *
  *�й��б��ṹ�����̣���μ� / libs / list.h��
  */
static void
stride_init(struct run_queue *rq) {

     list_init(&(rq->run_list));
     rq->lab6_run_pool = NULL;
     rq->proc_num = 0;
}

/*
  * stride_enqueue������``proc''�������ж���
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
stride_enqueue(struct run_queue *rq, struct proc_struct *proc) {

#if USE_SKEW_HEAP
     rq->lab6_run_pool =
          skew_heap_insert(rq->lab6_run_pool, &(proc->lab6_run_pool), proc_stride_comp_f);
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
  * stride_dequeue�����ж�����ɾ������``proc''
  *``rq''���ò�������skew_heap_remove���
  *������ ��סҪ����``rq''�ṹ��
  *
  *�й����ȼ������̣���μ� / libs / skew_heap.h
  *���нṹ��
  */
static void
stride_dequeue(struct run_queue *rq, struct proc_struct *proc) {

#if USE_SKEW_HEAP
     rq->lab6_run_pool =
          skew_heap_remove(rq->lab6_run_pool, &(proc->lab6_run_pool), proc_stride_comp_f);
#else
     assert(!list_empty(&(proc->run_link)) && proc->rq == rq);
     list_del_init(&(proc->run_link));
#endif
     rq->proc_num --;
}
/*
  * stride_pick_next��``���ж���''��ѡ��Ԫ�أ���ʹ��
  *��ȵ���Сֵ����������Ӧ�Ĺ���
  *ָ�롣 ����ָ�뽫�ɺ�le2proc���㣬
  *�йض��壬��μ�/ kern / process / proc.h�� �������NULL
  *������û�н��̡�
  *
  *ѡ��һ��proc�ṹʱ�����ס���²���
  * proc�����ԡ� ����+ = BIG_STRIDE /���ȼ���
  *
  *�й����ȼ������̣���μ�proj / libs / skew_heap.h
  *���нṹ��
  */
static struct proc_struct *
stride_pick_next(struct run_queue *rq) {

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
          if ((int32_t)(p->stride - q->stride) > 0)
               p = q;
          le = list_next(le);
     }
#endif
     if(p->pid !=2 && p->pid!=1)
	{
		cprintf("proc %d is running \n" ,p->pid);
		do_stride_queue(p->pid);
	}

     if (p->priority == 0)
          p->stride += BIG_STRIDE;
     else p->stride += BIG_STRIDE / p->priority;
     
     	
     return p;
}

/*
  * stride_proc_tick�뵱ǰ���̵�tick�¼�һ��ʹ�á� ��
  *Ӧ��鵱ǰ���̵�ʱ��Ƭ�Ƿ�Ϊ�� 
  *�þ�������proc�ṹ``proc''�� proc-> time_slice
  *��ʾ��ǰʣ���ʱ��Ƭ
  * ���̡� proc-> need_resched�ǽ��̵ı�־����
  *�л���
  */
static void
stride_proc_tick(struct run_queue *rq, struct proc_struct *proc) {

     if (proc->time_slice > 0) {
          proc->time_slice --;
     }
     if (proc->time_slice == 0) {
          proc->need_resched = 1;
     }
}

static int
stride_user_check(void){
	return 0;
}

struct sched_class stride_sched_class = {
     .name = "stride_scheduler",
     .init = stride_init,
     .enqueue = stride_enqueue,
     .dequeue = stride_dequeue,
     .pick_next = stride_pick_next,
     .proc_tick = stride_proc_tick,
     .user_check = stride_user_check,
};

