#include <defs.h>
#include <x86.h>
#include <stdio.h>
#include <string.h>
#include <swap.h>
#include <swap_clock.h>
#include <list.h>


list_entry_t pra_list_head;
/*
 * (2) _clock_init_mm：初始化pra_list_head并且使得mm->sm_priv指向pra_list_head的地址
 *                     通过mm_struct来控制clock策略
 */
static int
_clock_init_mm(struct mm_struct *mm)
{     
     list_init(&pra_list_head);
     mm->sm_priv = &pra_list_head;
     //cprintf(" mm->sm_priv %x in clock_init_mm\n",mm->sm_priv);
     return 0;
}
/*
 * (3)_clock_map_swappable：clock策略与FIFO策略一样，把最近使用的页加到队尾
 */
static int
_clock_map_swappable(struct mm_struct *mm, uintptr_t addr, struct Page *page, int swap_in)
{
    list_entry_t *head=(list_entry_t*) mm->sm_priv;
    list_entry_t *entry=&(page->pra_page_link);
 
    assert(entry != NULL && head != NULL);

    list_add(head, entry);
    return 0;
}
/*
 *  (4)_clock_swap_out_victim：根据Clock策略，我们把选定的页块弹出队列并把*ptr_page的值赋给这一页的地址
 */
static int
_clock_swap_out_victim(struct mm_struct *mm, struct Page ** ptr_page, int in_tick)
{
    list_entry_t *head=(list_entry_t*) mm->sm_priv;
        assert(head != NULL);
    assert(in_tick==0);

    // 第一次查找 !PTE_A & !PTE_D，同时重置当前页的PTE_A
    // 第二次查找 !PTE_A & !PTE_D， 同时重置当前页的PTE_D
    // 第三次查找，肯定能找到
    int i;
    for(i = 0; i < 3; i++)
    {
        list_entry_t *le = head->prev;
        assert(head!=le);
        while(le != head)
        {
            struct Page *p = le2page(le, pra_page_link);
            pte_t* ptep = get_pte(mm->pgdir, p->pra_vaddr, 0);
            // 如果满足未使用未修改这两个条件，则直接分配
            if(!(*ptep & PTE_A) && !(*ptep & PTE_D))
            {
                list_del(le);
                assert(p !=NULL);
                *ptr_page = p;
                return 0;
            }
            // 如果在第一次查找中，访问到了一个已经使用过的PTE，则标记为未使用。
            if(i == 0)
                *ptep &= ~PTE_A;
            // 如果在第二次查找中，访问到了一个已修改过的PTE，则标记为未修改。
            else if(i == 1)
                *ptep &= ~PTE_D;

            le = le->prev;
            // 遍历了一回，肯定修改了标志位，所以要刷新TLB
            tlb_invalidate(mm->pgdir, le);
        }
    }
    // 按照前面的assert与if，不可能会执行到此处，所以return -1
    return -1;
}

static int
_clock_check_swap(void) {
    cprintf("write Virt Page c in clock_check_swap\n");
    *(unsigned char *)0x3000 = 0x0c;
    assert(pgfault_num==4);
    cprintf("write Virt Page a in clock_check_swap\n");
    *(unsigned char *)0x1000 = 0x0a;
    assert(pgfault_num==4);
    cprintf("write Virt Page d in clock_check_swap\n");
    *(unsigned char *)0x4000 = 0x0d;
    assert(pgfault_num==4);
    cprintf("write Virt Page b in clock_check_swap\n");
    *(unsigned char *)0x2000 = 0x0b;
    assert(pgfault_num==4);
    cprintf("write Virt Page e in clock_check_swap\n");
    *(unsigned char *)0x5000 = 0x0e;
    assert(pgfault_num==5);
    cprintf("write Virt Page b in clock_check_swap\n");
    *(unsigned char *)0x2000 = 0x0b;
    assert(pgfault_num==5);
    cprintf("write Virt Page a in clock_check_swap\n");
    *(unsigned char *)0x1000 = 0x0a;
    assert(pgfault_num==6);
    cprintf("write Virt Page b in clock_check_swap\n");
    *(unsigned char *)0x2000 = 0x0b;
    assert(pgfault_num==6);
    cprintf("write Virt Page c in clock_check_swap\n");
    *(unsigned char *)0x3000 = 0x0c;
    assert(pgfault_num==7);
    cprintf("write Virt Page d in clock_check_swap\n");
    *(unsigned char *)0x4000 = 0x0d;
    assert(pgfault_num==8);
    cprintf("write Virt Page e in clock_check_swap\n");
    *(unsigned char *)0x5000 = 0x0e;
    assert(pgfault_num==8);
    cprintf("write Virt Page a in clock_check_swap\n");
    assert(*(unsigned char *)0x1000 == 0x0a);
    *(unsigned char *)0x1000 = 0x0a;
    assert(pgfault_num==8);
    return 0;
}


static int
_clock_user_check(void) {

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

    cprintf("write Virt Page a in clock_check_swap\n");
    *(unsigned char *)0x1000 = 0x0a;
    cprintf("    *(unsigned char *)0x1000 = 0x0a\n");
    assert(pgfault_num==6);
    cprintf("assert(pgfault_num==6)\n");

    cprintf("write Virt Page b in clock_check_swap\n");
    *(unsigned char *)0x2000 = 0x0b;
    cprintf("    *(unsigned char *)0x2000 = 0x0b\n");
    assert(pgfault_num==6);
    cprintf("assert(pgfault_num==6)\n");

    cprintf("write Virt Page c in clock_check_swap\n");
    *(unsigned char *)0x3000 = 0x0c;
    cprintf("    *(unsigned char *)0x3000 = 0x0c\n");
    assert(pgfault_num==7);
    cprintf("assert(pgfault_num==7)\n");

    cprintf("write Virt Page d in clock_check_swap\n");
    *(unsigned char *)0x4000 = 0x0d;
    cprintf("    *(unsigned char *)0x4000 = 0x0d\n");
    assert(pgfault_num==8);
    cprintf("assert(pgfault_num==8)\n");

    cprintf("write Virt Page e in clock_check_swap\n");
    *(unsigned char *)0x5000 = 0x0e;
    cprintf("    *(unsigned char *)0x5000 = 0x0e\n");
    assert(pgfault_num==8);
    cprintf("assert(pgfault_num==8)\n");

    cprintf("write Virt Page a in clock_check_swap\n");
    assert(*(unsigned char *)0x1000 == 0x0a);
    cprintf("    assert(*(unsigned char *)0x1000 == 0x0a)");
    *(unsigned char *)0x1000 = 0x0a;
    cprintf("    *(unsigned char *)0x1000 = 0x0a\n");
    assert(pgfault_num==8);
    cprintf("assert(pgfault_num==8)\n");

    return 0;
}

static int
_clock_init(void)
{
    return 0;
}

static int
_clock_set_unswappable(struct mm_struct *mm, uintptr_t addr)
{
    return 0;
}

static int
_clock_tick_event(struct mm_struct *mm)
{ return 0; }


struct swap_manager swap_manager_clock =
{
     .name            = "clock swap manager",
     .init            = &_clock_init,
     .init_mm         = &_clock_init_mm,
     .tick_event      = &_clock_tick_event,
     .map_swappable   = &_clock_map_swappable,
     .set_unswappable = &_clock_set_unswappable,
     .swap_out_victim = &_clock_swap_out_victim,
     .check_swap      = &_clock_check_swap,
     .user_check      = &_clock_user_check,
};
