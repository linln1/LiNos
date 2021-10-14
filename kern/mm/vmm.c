#include <vmm.h>
#include <sync.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <error.h>
#include <pmm.h>
#include <x86.h>
#include <swap.h>
#include <kmalloc.h>

/* 
  虚拟内存的设计包含两个部分：mm_struct (mm)和vma_struct (vma)
  mm是一系列逻辑上相连续的虚拟内存的管理数据结构，这些虚拟内存有相同的PDT
  vma是一个连续的虚拟内存区域
  mm有一个串起各个vma的线性链表
---------------
  与mm相关的函数
   全局函数
     struct mm_struct * mm_create(void)
     void mm_destroy(struct mm_struct *mm)
     int do_pgfault(struct mm_struct *mm, uint32_t error_code, uintptr_t addr)
--------------
  与vma相关的函数
   全局函数
     struct vma_struct * vma_create (uintptr_t vm_start, uintptr_t vm_end,...)
     void insert_vma_struct(struct mm_struct *mm, struct vma_struct *vma)
     struct vma_struct * find_vma(struct mm_struct *mm, uintptr_t addr)
   本文件函数
     inline void check_vma_overlap(struct vma_struct *prev, struct vma_struct *next)
---------------
   与用户交互且可以检测算法成败的函数
     void check_vmm(void);
     void check_vma_struct(void);
     void check_pgfault(void);
*/

uint32_t time_now = 0;
static void check_vmm(void);
static void check_vma_struct(void);
static void check_pgfault(void);

// 创建一个mm
struct mm_struct *
mm_create(void) {
    struct mm_struct *mm = kmalloc(sizeof(struct mm_struct));

    if (mm != NULL) {
        list_init(&(mm->mmap_list));
        mm->mmap_cache = NULL;
        mm->pgdir = NULL;
        mm->map_count = 0;
        mm->max_map = 10; //由于物理页的

        if (swap_init_ok) swap_init_mm(mm);
        else mm->sm_priv = NULL;
        
        set_mm_count(mm, 0);
        sem_init(&(mm->mm_sem), 1);
    }    
    return mm;
}

// 创建一个vma
struct vma_struct *
vma_create(uintptr_t vm_start, uintptr_t vm_end, uint32_t vm_flags) {
    struct vma_struct *vma = kmalloc(sizeof(struct vma_struct));

    if (vma != NULL) {
        vma->vm_start = vm_start;
        vma->vm_end = vm_end;
        vma->vm_flags = vm_flags;
        vma->last_used_time = 0;
    }
    return vma;
}


// 在mm管理的vma中定位需要的vma
struct vma_struct *
find_vma(struct mm_struct *mm, uintptr_t addr) {
    struct vma_struct *vma = NULL;
    if (mm != NULL) {
        vma = mm->mmap_cache;
        if (!(vma != NULL && vma->vm_start <= addr && vma->vm_end > addr)) {
                bool found = 0;
                list_entry_t *list = &(mm->mmap_list), *le = list;
                while ((le = list_next(le)) != list) {
                    vma = le2vma(le, list_link);
                    if (vma->vm_start<=addr && addr < vma->vm_end) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    vma = NULL;
                }
        }
        if (vma != NULL) {
            mm->mmap_cache = vma;
            vma->last_used_time = time_now;
        }
    }
    return vma;
}


// 检查两个vma是否有交集
static inline void
check_vma_overlap(struct vma_struct *prev, struct vma_struct *next) {
    assert(prev->vm_start < prev->vm_end);
    assert(prev->vm_end <= next->vm_start);
    assert(next->vm_start < next->vm_end);
}


// 在mm管理的vma链表中插入一个vma
void
insert_vma_struct(struct mm_struct *mm, struct vma_struct *vma) {
    assert(vma->vm_start < vma->vm_end);
    list_entry_t *list = &(mm->mmap_list);
    list_entry_t *le_prev = list, *le_next;

        list_entry_t *le = list;
        while ((le = list_next(le)) != list) {
            struct vma_struct *mmap_prev = le2vma(le, list_link);
            if (mmap_prev->vm_start > vma->vm_start) {
                break;
            }
            le_prev = le;
        }

    le_next = list_next(le_prev);

    /* 检查和此链表中的其他vma是否有交集 */
    if (le_prev != list) {
        check_vma_overlap(le2vma(le_prev, list_link), vma);
    }
    if (le_next != list) {
        check_vma_overlap(vma, le2vma(le_next, list_link));
    }

    vma->vm_mm = mm;
    list_add_after(le_prev, &(vma->list_link));

    mm->map_count ++;
}

// 释放一个mm
void
mm_destroy(struct mm_struct *mm) {
    assert(mm_count(mm) == 0);

    list_entry_t *list = &(mm->mmap_list), *le;
    while ((le = list_next(list)) != list) {
        list_del(le);
        kfree(le2vma(le, list_link));  //释放mm内的每一个vma       
    }
    kfree(mm); //释放这个mm
    mm=NULL;
}


//实现LRU算法的困难在于，这个操作系统没有为每个进程的能使用的物理页框数量做出限制，导致只有在缺页的时候才会去替换
//如果要使用LRU，就要给pra_list_head这个链表的大小做出限制，然后在链表项指向的结构上添加一个属性，标记着最近使用时间
int
mm_map(struct mm_struct *mm, uintptr_t addr, size_t len, uint32_t vm_flags,
       struct vma_struct **vma_store) {
    time_now++;
    uintptr_t start = ROUNDDOWN(addr, PGSIZE), end = ROUNDUP(addr + len, PGSIZE);
    if (!USER_ACCESS(start, end)) {
        return -E_INVAL;
    }

    assert(mm != NULL);

    int ret = -E_INVAL;

    struct vma_struct *vma;
    if ((vma = find_vma(mm, start)) != NULL && end > vma->vm_start) {
        //如果找到了现成的virtual memory page
        goto out;
    }
    ret = -E_NO_MEM;

    if ((vma = vma_create(start, end, vm_flags)) == NULL) {
        goto out;
    }
    insert_vma_struct(mm, vma);
    if (vma_store != NULL) {
        *vma_store = vma;
    }
    ret = 0;

out:
    return ret;
}

int
dup_mmap(struct mm_struct *to, struct mm_struct *from) {
    assert(to != NULL && from != NULL);
    list_entry_t *list = &(from->mmap_list), *le = list;
    while ((le = list_prev(le)) != list) {
        struct vma_struct *vma, *nvma;
        vma = le2vma(le, list_link);
        nvma = vma_create(vma->vm_start, vma->vm_end, vma->vm_flags);
        if (nvma == NULL) {
            return -E_NO_MEM;
        }

        insert_vma_struct(to, nvma);

        bool share = 0;
        if (copy_range(to->pgdir, from->pgdir, vma->vm_start, vma->vm_end, share) != 0) {
            return -E_NO_MEM;
        }
    }
    return 0;
}

void
exit_mmap(struct mm_struct *mm) {
    assert(mm != NULL && mm_count(mm) == 0);
    pde_t *pgdir = mm->pgdir;
    list_entry_t *list = &(mm->mmap_list), *le = list;
    while ((le = list_next(le)) != list) {
        struct vma_struct *vma = le2vma(le, list_link);
        unmap_range(pgdir, vma->vm_start, vma->vm_end);
    }
    while ((le = list_next(le)) != list) {
        struct vma_struct *vma = le2vma(le, list_link);
        exit_range(pgdir, vma->vm_start, vma->vm_end);
    }
}

bool
copy_from_user(struct mm_struct *mm, void *dst, const void *src, size_t len, bool writable) {
    if (!user_mem_check(mm, (uintptr_t)src, len, writable)) {
        return 0;
    }
    memcpy(dst, src, len);
    return 1;
}

bool
copy_to_user(struct mm_struct *mm, void *dst, const void *src, size_t len) {
    if (!user_mem_check(mm, (uintptr_t)dst, len, 1)) {
        return 0;
    }
    memcpy(dst, src, len);
    return 1;
}

// vmm_init - 初始化虚拟内存管理
//          - 再调用check_vmm来检查虚拟内存的正确性
void
vmm_init(void) {
    check_vmm();
}

// check_vmm - 检查vmm的正确性
static void
check_vmm(void) {
    size_t nr_free_pages_store = nr_free_pages();
    
    check_vma_struct();
    check_pgfault();

    //assert(nr_free_pages_store == nr_free_pages());

    //cprintf("check_vmm() succeeded.\n");
}

static void
check_vma_struct(void) {
    size_t nr_free_pages_store = nr_free_pages();

    struct mm_struct *mm = mm_create();
    assert(mm != NULL);

    int step1 = 10, step2 = step1 * 10;

    int i;
    for (i = step1; i >= 1; i --) {
        struct vma_struct *vma = vma_create(i * 5, i * 5 + 2, 0);
        assert(vma != NULL);
        insert_vma_struct(mm, vma);
    }

    for (i = step1 + 1; i <= step2; i ++) {
        struct vma_struct *vma = vma_create(i * 5, i * 5 + 2, 0);
        assert(vma != NULL);
        insert_vma_struct(mm, vma);
    }

    list_entry_t *le = list_next(&(mm->mmap_list));

    for (i = 1; i <= step2; i ++) {
        assert(le != &(mm->mmap_list));
        struct vma_struct *mmap = le2vma(le, list_link);
        assert(mmap->vm_start == i * 5 && mmap->vm_end == i * 5 + 2);
        le = list_next(le);
    }

    for (i = 5; i <= 5 * step2; i +=5) {
        struct vma_struct *vma1 = find_vma(mm, i);
        assert(vma1 != NULL);
        struct vma_struct *vma2 = find_vma(mm, i+1);
        assert(vma2 != NULL);
        struct vma_struct *vma3 = find_vma(mm, i+2);
        assert(vma3 == NULL);
        struct vma_struct *vma4 = find_vma(mm, i+3);
        assert(vma4 == NULL);
        struct vma_struct *vma5 = find_vma(mm, i+4);
        assert(vma5 == NULL);

        assert(vma1->vm_start == i  && vma1->vm_end == i  + 2);
        assert(vma2->vm_start == i  && vma2->vm_end == i  + 2);
    }

    for (i =4; i>=0; i--) {
        struct vma_struct *vma_below_5= find_vma(mm,i);
        if (vma_below_5 != NULL ) {
           cprintf("vma_below_5: i %x, start %x, end %x\n",i, vma_below_5->vm_start, vma_below_5->vm_end); 
        }
        assert(vma_below_5 == NULL);
    }

    mm_destroy(mm);

  //  assert(nr_free_pages_store == nr_free_pages());

    //cprintf("check_vma_struct() succeeded!\n");
}

struct mm_struct *check_mm_struct;

// check_pgfault - 检查页面异常中断的正确性
static void
check_pgfault(void) {
    size_t nr_free_pages_store = nr_free_pages();

    check_mm_struct = mm_create();
    assert(check_mm_struct != NULL);

    struct mm_struct *mm = check_mm_struct;
    pde_t *pgdir = mm->pgdir = boot_pgdir;
    assert(pgdir[0] == 0);

    struct vma_struct *vma = vma_create(0, PTSIZE, VM_WRITE);
    assert(vma != NULL);

    insert_vma_struct(mm, vma);

    uintptr_t addr = 0x100;
    assert(find_vma(mm, addr) == vma);

    int i, sum = 0;
    for (i = 0; i < 100; i ++) {
        *(char *)(addr + i) = i;
        sum += i;
    }
    for (i = 0; i < 100; i ++) {
        sum -= *(char *)(addr + i);
    }
    assert(sum == 0);

    page_remove(pgdir, ROUNDDOWN(addr, PGSIZE));
    free_page(pde2page(pgdir[0]));
    pgdir[0] = 0;

    mm->pgdir = NULL;
    mm_destroy(mm);
    check_mm_struct = NULL;

    assert(nr_free_pages_store == nr_free_pages());

    //cprintf("check_pgfault() succeeded!\n");
}
//页面错误的数量
volatile unsigned int pgfault_num=0;

/* do_pgfault - 处理缺页中断产生的异常
 * @mm         : 管理拥有相同PDT的vma集合
 * @error_code : 中断帧中的错误代码 -> tf_err由x86硬件设置
 * @addr       : 产生缺页中断的vma的地址，（CR2管理的内容）
 *
 * 调用关系: trap --> trap_dispatch --> pgfault_handler --> do_pgfault
 * 以下是便于do_pgfault处理缺页异常的硬件补充信息，且指出如何修改这些异常。
 *   (1) CR2注册表的内容：处理器向CR2注册表中添加如下内容
 *       产生中断的地址，32位表示。do_pgfault可以通过这个内容锁定与中断相关的页目录和页表入口。
 *   (2) 内核栈中的一个错误代码。页中断的错误码和其他中断的格式不一样。这个错误码可以给页中断处理器如下指示：
 *         -- P标志（第0位）为0则代表这个中断与目前未被使用的页无关；为1则代表违反访问权限或使用保留位。
 *         -- W/R标志（第1位）为0则代表引发此缺页中断的是个读操作；为1则代表是个写操作。
 *         -- U/S标志（第2位）为0则代表引发此缺页中断时进程处于用户态；为1则代表是内核态
 */
int
do_pgfault(struct mm_struct *mm, uint32_t error_code, uintptr_t addr) {
    int ret = -E_INVAL;
    //在mm中找到包含中断地址的vma
    struct vma_struct *vma = find_vma(mm, addr);

    pgfault_num++;
    //检查这个中断产生的地址是否是一个页的开始，作为是否用缺页中断处理函数来结局的依据。
    if (vma == NULL || vma->vm_start > addr) {
        cprintf("not valid addr %x, and  can not find it in vma\n", addr);
        if(vma == NULL){
            cprintf("because null!\n");
        }
        goto failed;
    }
    //检查错误码
    switch (error_code & 3) {
    default:
            /* 错误码标志：默认是3即（W/R=1，P=1）：写问题且正在被使用 */
    case 2: /* 错误码标志：（W/R=1，P=0）：写问题但目前未被使用 */
        if (!(vma->vm_flags & VM_WRITE)) {
            cprintf("do_pgfault failed: error code flag = write AND not present, but the addr's vma cannot write\n");
            goto failed;
        }
        break;
    case 1: /* 错误码标志：（W/R=0，P=1）：读问题且目前被使用 */
        cprintf("do_pgfault failed: error code flag = read AND present\n");
        goto failed;
    case 0: /* 错误码标志：（W/R=0，P=0）：读问题但目前未被使用 */
        if (!(vma->vm_flags & (VM_READ | VM_EXEC))) {
            cprintf("do_pgfault failed: error code flag = read AND not present, but the addr's vma cannot read or exec\n");
            goto failed;
        }
    }
    /* 如果（写一个已被使用的地址）或者
     *    （写一个未被使用的地址且地址是可写的）或者
     *    （读一个未被使用的地址且地址是可读的）
     * 就
     *    继续进程
     */
    uint32_t perm = PTE_U;
    if (vma->vm_flags & VM_WRITE) {
        perm |= PTE_W;
    }
    addr = ROUNDDOWN(addr, PGSIZE);

    ret = -E_NO_MEM;

    pte_t *ptep=NULL;

#if 0
    
    ptep = ???              //(1) 尝试找到pte，如果pte的PT（Page Table）不存在就创造一个PT
    if (*ptep == 0) {
                            //(2) 如果物理地址不存在，就分配一个物理页且把这个物理页和逻辑页对应上

    }
    else {


#endif
    // 尝试找到pte，如果pte的PT（Page Table）不存在就创造一个PT
    // 设置第三个参数为1
    if ((ptep = get_pte(mm->pgdir, addr, 1)) == NULL) {
        cprintf("get_pte in do_pgfault failed\n");
        goto failed;
    }
    
    if (*ptep == 0) { // 如果物理地址不存在，就分配一个物理页且把这个物理页和逻辑页对应上
        if (pgdir_alloc_page(mm->pgdir, addr, perm) == NULL) {
            cprintf("pgdir_alloc_page in do_pgfault failed\n");
            goto failed;
        }
    }
    else {
        struct Page *page=NULL;
        cprintf("do pgfault: ptep %x, pte %x\n",ptep, *ptep);
        if (*ptep & PTE_P) {
            // 如果进程在这个已存在的只读页上做了写操作（PTE_P代表已存在），那么这个页应该出现中断
            // 我们可以使用这个已经分配的内存空间复制给子进程（又叫做读复制，cow）
            // 目前现不复制，待会再复制
            panic("error write a non-writable pte");
            //page = pte2page(*ptep);
        } else{
            // 如果这个pte是一个交换区的入口，那么从硬盘上物理地址开始的地方加载数据到虚拟页
           // 并且调用page_insert把这个物理页和虚拟页建立映射
           if(swap_init_ok) {               
               if ((ret = swap_in(mm, addr, &page)) != 0) {
                   cprintf("swap_in in do_pgfault failed\n");
                   goto failed;
               }    

           }  
           else {
            cprintf("no swap_init_ok but ptep is %x, failed\n",*ptep);
            goto failed;
           }
       } 
       page_insert(mm->pgdir, page, addr, perm);
       swap_map_swappable(mm, addr, page, 1);
       page->pra_vaddr = addr;
   }
   ret = 0;
failed:
    return ret;
}

bool
user_mem_check(struct mm_struct *mm, uintptr_t addr, size_t len, bool write) {
    if (mm != NULL) {
        if (!USER_ACCESS(addr, addr + len)) {
            return 0;
        }
        struct vma_struct *vma;
        uintptr_t start = addr, end = addr + len;
        while (start < end) {
            if ((vma = find_vma(mm, start)) == NULL || start < vma->vm_start) {
                return 0;
            }
            if (!(vma->vm_flags & ((write) ? VM_WRITE : VM_READ))) {
                return 0;
            }
            if (write && (vma->vm_flags & VM_STACK)) {
                if (start < vma->vm_start + PGSIZE) { //check stack start & size
                    return 0;
                }
            }
            start = vma->vm_end;
        }
        return 1;
    }
    return KERN_ACCESS(addr, addr + len);
}

bool
copy_string(struct mm_struct *mm, char *dst, const char *src, size_t maxn) {
    size_t alen, part = ROUNDDOWN((uintptr_t)src + PGSIZE, PGSIZE) - (uintptr_t)src;
    while (1) {
        if (part > maxn) {
            part = maxn;
        }
        if (!user_mem_check(mm, (uintptr_t)src, part, 0)) {
            return 0;
        }
        if ((alen = strnlen(src, part)) < part) {
            memcpy(dst, src, alen + 1);
            return 1;
        }
        if (part == maxn) {
            return 0;
        }
        memcpy(dst, src, part);
        dst += part, src += part, maxn -= part;
        part = PGSIZE;
    }
}
