#ifndef __KERN_MM_VMM_H__
#define __KERN_MM_VMM_H__

#include <defs.h>
#include <list.h>
#include <memlayout.h>
#include <sync.h>
#include <proc.h>
#include <sem.h>

// 在vma_struct之上的一个数据结构
struct mm_struct;

// 虚拟连续内存区域(virtual continuously area -> vma)
struct vma_struct {
    struct mm_struct *vm_mm; // 回指到管理自己的mm -> mm管理一个拥有相同PDT的vma集合 
    uintptr_t vm_start;      // 此vma的起始地址
    uintptr_t vm_end;        // 此vma的结束地址
    uint32_t vm_flags;       // vma的标签，代表其是否被占用
    uint32_t last_used_time; // 记录这个vma最后一次被使用的时间，在LRU策略中会被使用
    list_entry_t list_link;  // 线性链表，把vma对应的地址串成一个线性表，由vm_start作为起始处
};

#define le2vma(le, member)                  \
    to_struct((le), struct vma_struct, member)

#define VM_READ                 0x00000001
#define VM_WRITE                0x00000002
#define VM_EXEC                 0x00000004
#define VM_STACK                0x00000008

// 管理vma的数据结构，mm_struct管理一个拥有相同PDT的vma集合
struct mm_struct {
    list_entry_t mmap_list;        // 储存此mm管理的各个vma的vm_start，以链表的方式管理
    struct vma_struct *mmap_cache; // 目前正在被使用的vma,设置cache加速进程的读写
    pde_t *pgdir;                  // 这些vma的PDT
    int map_count;                 // 这些vma一共的数量
    void *sm_priv;                 // swap_manager的本地数据
    int mm_count;                  // 目前有几个进程在共享这块内存
    semaphore_t mm_sem;            // 信号量，防止此块内存被随意读写 
    int locked_by;                 // 用户进程pid的锁
    int size_of_sm_priv;           // sm_priv的大小
    int max_map;                   // sm_priv的最大大小
};

struct vma_struct *find_vma(struct mm_struct *mm, uintptr_t addr);
struct vma_struct *vma_create(uintptr_t vm_start, uintptr_t vm_end, uint32_t vm_flags);
void insert_vma_struct(struct mm_struct *mm, struct vma_struct *vma);

struct mm_struct *mm_create(void);
void mm_destroy(struct mm_struct *mm);

void vmm_init(void);
int mm_map(struct mm_struct *mm, uintptr_t addr, size_t len, uint32_t vm_flags,
           struct vma_struct **vma_store);
int do_pgfault(struct mm_struct *mm, uint32_t error_code, uintptr_t addr);

int mm_unmap(struct mm_struct *mm, uintptr_t addr, size_t len);
int dup_mmap(struct mm_struct *to, struct mm_struct *from);
void exit_mmap(struct mm_struct *mm);
uintptr_t get_unmapped_area(struct mm_struct *mm, size_t len);
int mm_brk(struct mm_struct *mm, uintptr_t addr, size_t len);

extern volatile unsigned int pgfault_num;
extern struct mm_struct *check_mm_struct;

bool user_mem_check(struct mm_struct *mm, uintptr_t start, size_t len, bool write);
bool copy_from_user(struct mm_struct *mm, void *dst, const void *src, size_t len, bool writable);
bool copy_to_user(struct mm_struct *mm, void *dst, const void *src, size_t len);
bool copy_string(struct mm_struct *mm, char *dst, const char *src, size_t maxn);

static inline int
mm_count(struct mm_struct *mm) {
    return mm->mm_count;
}

static inline void
set_mm_count(struct mm_struct *mm, int val) {
    mm->mm_count = val;
}

static inline int
mm_count_inc(struct mm_struct *mm) {
    mm->mm_count += 1;
    return mm->mm_count;
}

static inline int
mm_count_dec(struct mm_struct *mm) {
    mm->mm_count -= 1;
    return mm->mm_count;
}

static inline void
lock_mm(struct mm_struct *mm) {
    if (mm != NULL) {
        down(&(mm->mm_sem));
        if (current != NULL) {
            mm->locked_by = current->pid;
        }
    }
}

static inline void
unlock_mm(struct mm_struct *mm) {
    if (mm != NULL) {
        up(&(mm->mm_sem));
        mm->locked_by = 0;
    }
}

#endif /* !__KERN_MM_VMM_H__ */

