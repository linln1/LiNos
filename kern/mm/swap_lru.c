#include <defs.h>
#include <x86.h>
#include <stdio.h>
#include <string.h>
#include <swap.h>
#include <swap_lru.h>
#include <list.h>


list_entry_t pra_list_head;
/*
 * (2) _lru_init_mm：初始化pra_list_head并且使得mm->sm_priv指向pra_list_head的地址
 *                   我们通过mm_struct来操作LRU策略
 */
static int
_lru_init_mm(struct mm_struct *mm)
{     
     list_init(&pra_list_head);
     mm->sm_priv = &pra_list_head;
     //cprintf(" mm->sm_priv %x in lru_init_mm\n",mm->sm_priv);
     mm->size_of_sm_priv = 0; 
     mm->max_map = 10;
     return 0;
}
/*
 * (3)_lru_map_swappable：根据LRU策略，我们应该把最近使用的页块添加到pra_list_head这个队列的队头
 */
static int
_lru_map_swappable(struct mm_struct *mm, uintptr_t addr, struct Page *page, int swap_in)
{
    list_entry_t *head=(list_entry_t*) mm->sm_priv;
    lock_mm(mm);
    {
        if(mm->size_of_sm_priv < mm->max_map){
            //如果当前使用的物理页框还足够，就继续在head里面添加
        }else{
            //否则在这里进行调度, 查找mm对应的vma_list里面最久未使用的虚拟页对应的物理页，然后换掉它

            //取出mmap_list的头节点
            list_entry_t *le = &mm->mmap_list;
            list_entry_t *vma_head = &mm->mmap_list;
            struct vma_struct *change_vma;
            struct vma_struct *temp_vma;
            uint32_t last_used_time = 0;
            while((le = list_next(le) != vma_head)){
                temp_vma = le2vma(le, list_link);
                if(last_used_time > temp_vma->last_used_time){
                    change_vma = temp_vma;                  //找到 pra_list_head 里面映射到这个地址的物理页，然后page和这一页换掉
                }
            }

            list_entry_t *ple = &pra_list_head;
            while((ple = list_next(ple)) != &pra_list_head){
                struct Page *victim = le2page(ple, page_link);
                if(victim->pra_vaddr == change_vma->vm_start){//找到了该需要替换的物理页
                    list_del(&(victim->page_link));
                }
                
            }
            
            return 0;
        }
    }
    unlock_mm(mm);
    
    list_entry_t *entry=&(page->pra_page_link);
 
    assert(entry != NULL && head != NULL);
    //record the page access situlation
    list_add(head, entry);
    lock_mm(mm);
    {
        mm->size_of_sm_priv++;  //物理页的个数+1
    }
    unlock_mm(mm);
    return 0;
}
/*
 *  (4)_lru_swap_out_victim：根据LRU策略，我们把队尾的页块弹出队列并把*ptr_page的值赋给这一页的地址
 */
static int
_lru_swap_out_victim(struct mm_struct *mm, struct Page ** ptr_page, int in_tick)
{
    list_entry_t *head = (list_entry_t*)mm->sm_priv;
    assert(head != NULL);
    assert(in_tick == 0);

    list_entry_t *le = &mm->mmap_list;
    list_entry_t *vma_head = &mm->mmap_list;
    struct vma_struct *change_vma;
    struct vma_struct *temp_vma;
    uint32_t last_used_time = 0;
    while((le = list_next(le) != vma_head)){
        temp_vma = le2vma(le, list_link);
        if(last_used_time > temp_vma->last_used_time){
            change_vma = temp_vma;                  //找到 pra_list_head 里面映射到这个地址的物理页，然后page和这一页换掉
        }
    }

    list_entry_t *ple = &pra_list_head;
    struct Page *victim;
    while((ple = list_next(ple)) != &pra_list_head){
        victim = le2page(ple, page_link);
        if(victim->pra_vaddr == change_vma->vm_start){//找到了该需要替换的物理页
            list_del(&(victim->page_link));
        }
        assert(victim !=NULL);
    }

    *ptr_page = victim;

    lock_mm(mm);
    {
        mm->size_of_sm_priv--;  //物理页的个数-1
    }
    unlock_mm(mm);
    
    return 0;
}

static int
_lru_check_swap(void) {
    cprintf("write Virt Page c in lru_check_swap\n");
    *(unsigned char *)0x3000 = 0x0c;
    assert(pgfault_num==4);
    cprintf("write Virt Page a in lru_check_swap\n");
    *(unsigned char *)0x1000 = 0x0a;
    assert(pgfault_num==4);
    cprintf("write Virt Page d in lru_check_swap\n");
    *(unsigned char *)0x4000 = 0x0d;
    assert(pgfault_num==4);
    cprintf("write Virt Page b in lru_check_swap\n");
    *(unsigned char *)0x2000 = 0x0b;
    assert(pgfault_num==4);
    cprintf("write Virt Page e in lru_check_swap\n");
    *(unsigned char *)0x5000 = 0x0e;
    assert(pgfault_num==5);
    cprintf("write Virt Page b in lru_check_swap\n");
    *(unsigned char *)0x2000 = 0x0b;
    assert(pgfault_num==5);
    cprintf("write Virt Page a in lru_check_swap\n");
    *(unsigned char *)0x1000 = 0x0a;
    assert(pgfault_num==6);
    cprintf("write Virt Page b in lru_check_swap\n");
    *(unsigned char *)0x2000 = 0x0b;
    assert(pgfault_num==7);
    cprintf("write Virt Page c in lru_check_swap\n");
    *(unsigned char *)0x3000 = 0x0c;
    assert(pgfault_num==8);
    cprintf("write Virt Page d in lru_check_swap\n");
    *(unsigned char *)0x4000 = 0x0d;
    assert(pgfault_num==9);
    cprintf("write Virt Page e in lru_check_swap\n");
    *(unsigned char *)0x5000 = 0x0e;
    assert(pgfault_num==10);
    cprintf("write Virt Page a in lru_check_swap\n");
    assert(*(unsigned char *)0x1000 == 0x0a);
    *(unsigned char *)0x1000 = 0x0a;
    assert(pgfault_num==11);
    return 0;
}

static int
_lru_user_check(void) {

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
_lru_init(void)
{
    return 0;
}

static int
_lru_set_unswappable(struct mm_struct *mm, uintptr_t addr)
{
    return 0;
}

static int
_lru_tick_event(struct mm_struct *mm)
{ return 0; }


struct swap_manager swap_manager_lru =
{
     .name            = "lru swap manager",
     .init            = &_lru_init,
     .init_mm         = &_lru_init_mm,
     .tick_event      = &_lru_tick_event,
     .map_swappable   = &_lru_map_swappable,
     .set_unswappable = &_lru_set_unswappable,
     .swap_out_victim = &_lru_swap_out_victim,
     .check_swap      = &_lru_check_swap,
     .user_check      = &_lru_user_check,
};
