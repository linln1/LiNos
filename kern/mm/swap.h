#ifndef __KERN_MM_SWAP_H__
#define __KERN_MM_SWAP_H__

#include <defs.h>
#include <memlayout.h>
#include <pmm.h>
#include <vmm.h>

//此结构的介绍看报告
/* *
 * swap_entry_t
 * --------------------------------------------
 * |         offset        |   reserved   | 0 |
 * --------------------------------------------
 *           24 bits            7 bits    1 bit
 * */

#define MAX_SWAP_OFFSET_LIMIT                   (1 << 24)

extern size_t max_swap_offset;

/* *
 * swap_offset - 利用swap_entry (pte里的一位), 计算相关的交换偏移量
 * */
#define swap_offset(entry) ({                                       \
               size_t __offset = (entry >> 8);                        \
               if (!(__offset > 0 && __offset < max_swap_offset)) {    \
                    panic("invalid swap_entry_t = %08x.\n", entry);    \
               }                                                    \
               __offset;                                            \
          })

struct swap_manager
{
     const char *name;
     /* 替换管理者的初始化 */
     int (*init)            (void);
     /* 初始化它自己的本地内容 */
     int (*init_mm)         (struct mm_struct *mm);
     /* 当有事件发生时转向其他事件 */
     int (*tick_event)      (struct mm_struct *mm);
     /* 把一个可替换的页面加入到mm_struct */
     int (*map_swappable)   (struct mm_struct *mm, uintptr_t addr, struct Page *page, int swap_in);
     /* 当一个页面地址被共享时调用这个函数删除这个页面在管理中的入口 */
     int (*set_unswappable) (struct mm_struct *mm, uintptr_t addr);
     /* 替换页面且返回新的页面 */
     int (*swap_out_victim) (struct mm_struct *mm, struct Page **ptr_page, int in_tick);
     /* 检查算法是否成功 */
     int (*check_swap)(void);     
     int (*user_check)(void);
};

extern const struct swap_manager *sm;
extern volatile int swap_init_ok;
int swap_init(void);
int swap_init_mm(struct mm_struct *mm);
int swap_tick_event(struct mm_struct *mm);
int swap_map_swappable(struct mm_struct *mm, uintptr_t addr, struct Page *page, int swap_in);
int swap_set_unswappable(struct mm_struct *mm, uintptr_t addr);
int swap_out(struct mm_struct *mm, int n, int in_tick);
int swap_in(struct mm_struct *mm, uintptr_t addr, struct Page **ptr_result);
void do_change_swap(unsigned int mode);

//#define MEMBER_OFFSET(m,t) ((int)(&((t *)0)->m))
//#define FROM_MEMBER(m,t,a) ((t *)((char *)(a) - MEMBER_OFFSET(m,t)))

#endif
