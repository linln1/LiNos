#include <defs.h>
#include <x86.h>
#include <stdio.h>
#include <string.h>
#include <swap.h>
#include <swap_fifo.h>
#include <list.h>

/* 最简的页面替换算法就是FIFO策略，FIFO指最先进的最后出
 * 页面替换算法是一个低开销算法并只需要操作系统为其开辟少量运行空间
 * 操作系统一直跟踪正在使用的虚拟页块，把它们维护成一个队列，最先来的在队头，最后来的在队尾 
 * 当产生缺页中断且需要替换时，弹出队头的页，把新页加入队尾即可
 * 因为FIFO过于简单故其效果并不是太好，也因此很少被以未修改的模式使用（例如我们在其基础上实现CLOCK策略）
 * 此算法经常遇到Belady异常
 *
 * FIFO页面替换策略的细节
 * (1) 预备：为了实现FIFO页面替换策略，我们需要管理所有可替换的页面，所有我们按照时序把这些页面加入pra_list_head这个链表
 *            此处我们会用到list.h中的list，它是一个双端链表的简单实现 
 *            与它相关的函数有：list_init，list_add(list_add_after)，list_add_before，list_del，list_next，list_prev
 *            另一个方法就是把常规的链表转换为一个特殊结构，例如：struct page
 *            其他例子：le2page在memlayout.h，le2vma在vmm.h，le2proc在proc.h
 */

list_entry_t pra_list_head;
/*
 * (2) _fifo_init_mm：初始化pra_list_head并且使得mm->sm_priv指向pra_list_head的地址
 *                    我们通过mm_struct来操作FIFO策略
 */
static int
_fifo_init_mm(struct mm_struct *mm)
{     
     list_init(&pra_list_head);
     mm->sm_priv = &pra_list_head;
     //cprintf(" mm->sm_priv %x in fifo_init_mm\n",mm->sm_priv);
     return 0;
}
/*
 * (3)_fifo_map_swappable：根据FIFO策略，我们应该把最近使用的页块添加到pra_list_head这个队列的队尾
 */
static int
_fifo_map_swappable(struct mm_struct *mm, uintptr_t addr, struct Page *page, int swap_in)
{
    list_entry_t *head=(list_entry_t*) mm->sm_priv;
    list_entry_t *entry=&(page->pra_page_link);
 
    assert(entry != NULL && head != NULL);

    //(1)把最近使用的页块添加到pra_list_head这个队列的队尾
    list_add(head, entry);
    return 0;
}
/*
 *  (4)_fifo_swap_out_victim：根据FIFO策略，我们把队头的页块弹出队列并把*ptr_page的值赋给这一页的地址
 */
static int
_fifo_swap_out_victim(struct mm_struct *mm, struct Page ** ptr_page, int in_tick)
{
     list_entry_t *head=(list_entry_t*) mm->sm_priv;
         assert(head != NULL);
     assert(in_tick==0);
     /* 选到要换掉的页块 */

     /* 选到队尾 */
     list_entry_t *le = head->prev;
     assert(head!=le);
     struct Page *p = le2page(le, pra_page_link);
     list_del(le);
     assert(p !=NULL);
     *ptr_page = p;
     return 0;
}

static int
_fifo_check_swap(void) {
    cprintf("write Virt Page c in fifo_check_swap\n");
    *(unsigned char *)0x3000 = 0x0c;
    assert(pgfault_num==4);
    cprintf("write Virt Page a in fifo_check_swap\n");
    *(unsigned char *)0x1000 = 0x0a;
    assert(pgfault_num==4);
    cprintf("write Virt Page d in fifo_check_swap\n");
    *(unsigned char *)0x4000 = 0x0d;
    assert(pgfault_num==4);
    cprintf("write Virt Page b in fifo_check_swap\n");
    *(unsigned char *)0x2000 = 0x0b;
    assert(pgfault_num==4);
    cprintf("write Virt Page e in fifo_check_swap\n");
    *(unsigned char *)0x5000 = 0x0e;
    assert(pgfault_num==5);
    cprintf("write Virt Page b in fifo_check_swap\n");
    *(unsigned char *)0x2000 = 0x0b;
    assert(pgfault_num==5);
    cprintf("write Virt Page a in fifo_check_swap\n");
    *(unsigned char *)0x1000 = 0x0a;
    assert(pgfault_num==6);
    cprintf("write Virt Page b in fifo_check_swap\n");
    *(unsigned char *)0x2000 = 0x0b;
    assert(pgfault_num==7);
    cprintf("write Virt Page c in fifo_check_swap\n");
    *(unsigned char *)0x3000 = 0x0c;
    assert(pgfault_num==8);
    cprintf("write Virt Page d in fifo_check_swap\n");
    *(unsigned char *)0x4000 = 0x0d;
    assert(pgfault_num==9);
    cprintf("write Virt Page e in fifo_check_swap\n");
    *(unsigned char *)0x5000 = 0x0e;
    assert(pgfault_num==10);
    cprintf("write Virt Page a in fifo_check_swap\n");
    assert(*(unsigned char *)0x1000 == 0x0a);
    *(unsigned char *)0x1000 = 0x0a;
    assert(pgfault_num==11);
    return 0;
}


static int
_fifo_user_check(void) {

    cprintf("write Virt Page c in clock_check_swap\n");
    *(unsigned char *)0x3000 = 0x0c;
    cprintf("    *(unsigned char *)0x3000 = 0x0c\n");
    assert(pgfault_num==4);
    cprintf("assert(pgfault_num==4)\n");

    cprintf("write Virt Page a in clock_check_swap\n");
    *(unsigned char *)0x1000 = 0x0a;
    cprintf("    *(unsigned char *)0x1000 = 0x0a\n");
    assert(pgfault_num==4);
    cprintf("assert(pgfault_num==4)\n");

    cprintf("write Virt Page d in clock_check_swap\n");
    *(unsigned char *)0x4000 = 0x0d;
    cprintf("    *(unsigned char *)0x4000 = 0x0d\n");
    assert(pgfault_num==4);
    cprintf("assert(pgfault_num==4)\n");

    cprintf("write Virt Page b in clock_check_swap\n");
    *(unsigned char *)0x2000 = 0x0b;
    cprintf("    *(unsigned char *)0x2000 = 0x0b\n");
    assert(pgfault_num==4);
    cprintf("assert(pgfault_num==4)\n");

    cprintf("write Virt Page e in clock_check_swap\n");
    *(unsigned char *)0x5000 = 0x0e;
    cprintf("    *(unsigned char *)0x5000 = 0x0e\n");
    assert(pgfault_num==5);
    cprintf("assert(pgfault_num==5)\n");

    cprintf("write Virt Page b in clock_check_swap\n");
    *(unsigned char *)0x2000 = 0x0b;
    cprintf("    *(unsigned char *)0x2000 = 0x0b\n");
    assert(pgfault_num==5);
    cprintf("assert(pgfault_num==5)\n");

    cprintf("write Virt Page a in fifo_check_swap\n");
    *(unsigned char *)0x1000 = 0x0a;
    cprintf("    *(unsigned char *)0x1000 = 0x0a\n");
    assert(pgfault_num==6);
    cprintf("assert(pgfault_num==6)\n");

    cprintf("write Virt Page b in fifo_check_swap\n");
    *(unsigned char *)0x2000 = 0x0b;
    cprintf("    *(unsigned char *)0x2000 = 0x0b\n");
    assert(pgfault_num==7);
    cprintf("assert(pgfault_num==7)\n");

    cprintf("write Virt Page c in fifo_check_swap\n");
    *(unsigned char *)0x3000 = 0x0c;
    cprintf("    *(unsigned char *)0x3000 = 0x0c\n");
    assert(pgfault_num==8);
    cprintf("assert(pgfault_num==8)\n");

    cprintf("write Virt Page d in fifo_check_swap\n");
    *(unsigned char *)0x4000 = 0x0d;
    cprintf("    *(unsigned char *)0x4000 = 0x0d\n");
    assert(pgfault_num==9);
    cprintf("assert(pgfault_num==9)\n");

    cprintf("write Virt Page e in fifo_check_swap\n");
    *(unsigned char *)0x5000 = 0x0e;
    cprintf("    *(unsigned char *)0x5000 = 0x0e\n");
    assert(pgfault_num==10);
    cprintf("assert(pgfault_num==10)\n");

    cprintf("write Virt Page a in fifo_check_swap\n");
    assert(*(unsigned char *)0x1000 == 0x0a);
    cprintf("assert(*(unsigned char *)0x1000 == 0x0a)\n");
    *(unsigned char *)0x1000 = 0x0a;
    cprintf("    *(unsigned char *)0x1000 = 0x0a\n");
    assert(pgfault_num==11);
    cprintf("assert(pgfault_num==11)\n");

    return 0;
}


static int
_fifo_init(void)
{
    return 0;
}

static int
_fifo_set_unswappable(struct mm_struct *mm, uintptr_t addr)
{
    return 0;
}

static int
_fifo_tick_event(struct mm_struct *mm)
{ return 0; }


struct swap_manager swap_manager_fifo =
{
     .name            = "fifo swap manager",
     .init            = &_fifo_init,
     .init_mm         = &_fifo_init_mm,
     .tick_event      = &_fifo_tick_event,
     .map_swappable   = &_fifo_map_swappable,
     .set_unswappable = &_fifo_set_unswappable,
     .swap_out_victim = &_fifo_swap_out_victim,
     .check_swap      = &_fifo_check_swap,
     .user_check      = &_fifo_user_check,
};
