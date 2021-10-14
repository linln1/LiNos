#include <defs.h>
#include <x86.h>
#include <stdio.h>
#include <string.h>
#include <mmu.h>
#include <memlayout.h>
#include <pmm.h>
#include <firstfit_pmm.h>
#include <bestfit_pmm.h>
#include <nextfit_pmm.h>
#include <buddy_pmm.h>
#include <sync.h>
#include <error.h>
#include <swap.h>
#include <vmm.h>
#include <kmalloc.h>


static struct taskstate ts = {0};

// 物理页数组的虚拟地址
struct Page *pages;
// 物理内存数量(以页为单位)
size_t npage = 0;

// 引导时页目录的虚拟地址
extern pde_t __boot_pgdir;
pde_t *boot_pgdir = &__boot_pgdir;
// 启动时页面目录的物理地址
uintptr_t boot_cr3;

// 物理内存管理器
const struct pmm_manager *pmm_manager;

/* *
*对应于虚拟地址范围的页目录条目
* [VPT, VPT + PTSIZE)指向页面目录本身。因此,该页面
* directory既作为页表，也作为页目录。
*
*将页目录作为页表处理的结果之一是所有的pte
*可以通过“虚拟页表”访问虚拟地址VPT。和
*数字n的PTE存储在vpt[n]中。
*
*第二个结果是，当前页面目录的内容将在虚拟地址PGADDR(PDX(VPT)， PDX(VPT)， 0)中始终可用
*下面设置VPD。
* */
pte_t * const vpt = (pte_t *)VPT;
pde_t * const vpd = (pde_t *)PGADDR(PDX(VPT), PDX(VPT), 0);

/* *
*全局描述符表:
*
*内核段和用户段是相同的(除了DPL)。加载
* %ss寄存器，CPL必须等于DPL。因此，我们必须复制
*用于用户和内核的段。定义如下:
* - 0x0:未使用(总是错误——用于捕获NULL远指针)
* - 0x8:内核代码段
* - 0x10:内核数据段
* - 0x18:用户代码段
* - 0x20:用户数据段
* - 0x28:为tss定义，在gdt_init中初始化
* */
static struct segdesc gdt[] = {
    SEG_NULL,
    [SEG_KTEXT] = SEG(STA_X | STA_R, 0x0, 0xFFFFFFFF, DPL_KERNEL),
    [SEG_KDATA] = SEG(STA_W, 0x0, 0xFFFFFFFF, DPL_KERNEL),
    [SEG_UTEXT] = SEG(STA_X | STA_R, 0x0, 0xFFFFFFFF, DPL_USER),
    [SEG_UDATA] = SEG(STA_W, 0x0, 0xFFFFFFFF, DPL_USER),
    [SEG_TSS]   = SEG_NULL,
};

static struct pseudodesc gdt_pd = {
    sizeof(gdt) - 1, (uintptr_t)gdt
};

//static void check_alloc_page(void);
//static void check_pgdir(void);
//static void check_boot_pgdir(void);

/* *
* LGDT -加载全局描述符表寄存器并重置
内核的数据/代码段寄存器。
* */
static inline void
lgdt(struct pseudodesc *pd) {
    asm volatile ("lgdt (%0)" :: "r" (pd));
    asm volatile ("movw %%ax, %%gs" :: "a" (USER_DS));
    asm volatile ("movw %%ax, %%fs" :: "a" (USER_DS));
    asm volatile ("movw %%ax, %%es" :: "a" (KERNEL_DS));
    asm volatile ("movw %%ax, %%ds" :: "a" (KERNEL_DS));
    asm volatile ("movw %%ax, %%ss" :: "a" (KERNEL_DS));
    // reload cs
    asm volatile ("ljmp %0, $1f\n 1:\n" :: "i" (KERNEL_CS));
}

/* *
* load_esp0 -在默认任务状态段中修改ESP0，
* 以便当我们从用户态到内核态捕获到帧时，我们可以使用不同的内核堆栈，
* */
void
load_esp0(uintptr_t esp0) {
    ts.ts_esp0 = esp0;
}

/* gt_init -初始化默认的GDT和TSS */
static void
gdt_init(void) {
    //设置启动内核栈和默认SS0
    load_esp0((uintptr_t)bootstacktop);
    ts.ts_ss0 = KERNEL_DS;

    //初始化gdt的TSS文件
    gdt[SEG_TSS] = SEGTSS(STS_T32A, (uintptr_t)&ts, sizeof(ts), DPL_KERNEL);

   //重新加载所有段寄存器
    lgdt(&gdt_pd);

    //加载TSS
    ltr(GD_TSS);
}

//init_pmm_manager -初始化pmm_manager实例
static void
init_pmm_manager(void) {
    pmm_manager = &firstfit_pmm_manager;
    //cprintf("memory management: %s\n", pmm_manager->name);
    pmm_manager->init();
}

//init_memmap - 调用pmm->init_memmap来构建Page结构体以释放内存
static void
init_memmap(struct Page *base, size_t n) {
    pmm_manager->init_memmap(base, n);
}

//调用pmm->alloc_pages分配一个连续的n*PAGESIZE内存
struct Page *
alloc_pages(size_t n) {
    struct Page *page=NULL;
    bool intr_flag;
    
    while (1)
    {
         local_intr_save(intr_flag);
         {
              page = pmm_manager->alloc_pages(n);
         }
         local_intr_restore(intr_flag);

         if (page != NULL || n > 1 || swap_init_ok == 0) break;
         
         extern struct mm_struct *check_mm_struct;
         //cprintf("page %x, call swap_out in alloc_pages %d\n",page, n);
         swap_out(check_mm_struct, n, 0);
    }
    //cprintf("n %d,get page %x, No %d in alloc_pages\n",n,page,(page-pages));
    return page;
}

//free_pages -调用pmm->free_pages释放一个连续的n*PAGESIZE内存
void
free_pages(struct Page *base, size_t n) {
    bool intr_flag;
    local_intr_save(intr_flag);
    {
        pmm_manager->free_pages(base, n);
    }
    local_intr_restore(intr_flag);
}

//nr_free_pages -调用pmm->nr_free_pages来获取当前空闲内存的大小(nr*PAGESIZE)
size_t
nr_free_pages(void) {
    size_t ret;
    bool intr_flag;
    local_intr_save(intr_flag);
    {
        ret = pmm_manager->nr_free_pages();
    }
    local_intr_restore(intr_flag);
    return ret;
}

/* pmm_init -初始化物理内存管理*/
static void
page_init(void) {
    struct e820map *memmap = (struct e820map *)(0x8000 + KERNBASE);
    uint64_t maxpa = 0;
    cprintf("\n2.打印物理内存分布\n");
    cprintf("max physical space %x\n", KMEMSIZE);
    cprintf("e820map:\n");
    int i;
    for (i = 0; i < memmap->nr_map; i ++) {
        uint64_t begin = memmap->map[i].addr, end = begin + memmap->map[i].size;
        cprintf("  memory: %08llx, [%08llx, %08llx], type = %d.\n",
                memmap->map[i].size, begin, end - 1, memmap->map[i].type);
        if (memmap->map[i].type == E820_ARM) {
            if (maxpa < end && begin < KMEMSIZE) {
                maxpa = end;
            }
        }
    }
    if (maxpa > KMEMSIZE) {
        maxpa = KMEMSIZE;
    }

    extern char end[];

    npage = maxpa / PGSIZE;
    pages = (struct Page *)ROUNDUP((void *)end, PGSIZE);

    for (i = 0; i < npage; i ++) {
        SetPageReserved(pages + i);
    }

    uintptr_t freemem = PADDR((uintptr_t)pages + sizeof(struct Page) * npage);

    for (i = 0; i < memmap->nr_map; i ++) {
        uint64_t begin = memmap->map[i].addr, end = begin + memmap->map[i].size;
        if (memmap->map[i].type == E820_ARM) {
            if (begin < freemem) {
                begin = freemem;
            }
            if (end > KMEMSIZE) {
                end = KMEMSIZE;
            }
            if (begin < end) {
                begin = ROUNDUP(begin, PGSIZE);
                end = ROUNDDOWN(end, PGSIZE);
                if (begin < end) {
                    init_memmap(pa2page(begin), (end - begin) / PGSIZE);
                }
            }
        }
    }
}

//boot_map_segment - 设置并启用分页机制
// 参数
//  la:   该内存需要映射的线性地址(x86段映射之后)
//  size: 内存大小
//  pa:   该内存的物理地址
//  perm: 该内存可用许可
static void
boot_map_segment(pde_t *pgdir, uintptr_t la, size_t size, uintptr_t pa, uint32_t perm) {
    assert(PGOFF(la) == PGOFF(pa));
    size_t n = ROUNDUP(size + PGOFF(la), PGSIZE) / PGSIZE;
    la = ROUNDDOWN(la, PGSIZE);
    pa = ROUNDDOWN(pa, PGSIZE);
    for (; n > 0; n --, la += PGSIZE, pa += PGSIZE) {
        pte_t *ptep = get_pte(pgdir, la, 1);
        assert(ptep != NULL);
        *ptep = pa | PTE_P | perm;
    }
}

//boot_alloc_page - 使用pmm->alloc_pages(1)来分配一个页
//返回值:这个被分配页面的内核虚拟地址
//注意:此函数用于获取页目录表PDT和页表pt的内存。
static void *
boot_alloc_page(void) {
    struct Page *p = alloc_page();
    if (p == NULL) {
        panic("boot_alloc_page failed.\n");
    }
    return page2kva(p);
}

//pmm_init - 设置一个pmm来管理物理内存，构建PDT&PT来设置分页机制
//         - 检查pmm和分页机制的正确性，打印PDT&PT
void
pmm_init(void) {
    // We've already enabled paging
    boot_cr3 = PADDR(boot_pgdir);

    // 我们需要分配/释放物理内存(大小为4KB或其他大小)。
    // 在pmm.h中定义了一个物理内存管理器结构体(struct pmm_manager)
    // 首先初始化一个物理内存管理器(pmm)。
    // 之后pmm可以分配/释放物理内存。
    // 现在first_fit/best_fit/worst_fit/buddy_system pmm可用。
    init_pmm_manager();

    // 检测物理内存空间，预留已经使用的内存，
    // 之后使用pmm->init_memmap创建空闲页面列表
    page_init();

    //检查pmm中alloc/free函数的正确性
    check_alloc_page();

    check_pgdir();

    static_assert(KERNBASE % PTSIZE == 0 && KERNTOP % PTSIZE == 0);

    //递归插入boot_pgdir本身
    //在虚拟地址VPT上形成一个虚拟页表
    boot_pgdir[PDX(VPT)] = PADDR(boot_pgdir) | PTE_P | PTE_W;

    // 使用KERNBASE地址将所有物理内存映射为线性内存
    // 线性地址 KERNBASE ~ KERNBASE + KMEMSIZE = phy_addr 0 ~ KMEMSIZE
    boot_map_segment(boot_pgdir, KERNBASE, KMEMSIZE, 0, PTE_W);

    // 因为我们使用引导加载程序的GDT，
    // 我们应该重新加载gdt(第二次，最后一次)来获得用户段和TSS
    // 映射virtual_addr 0 ~ 4G = linear_addr 0 ~ 4G
    // 在TSS中设置内核栈(ss:esp)，在gdt中设置TSS，加载TSS
    gdt_init();

    // 现在基本的虚拟内存映射(见memalyout.h)已经建立。
    // 检查基本虚拟内存映射的正确性。
    check_boot_pgdir();
    
    cprintf("\n3. 打印初始页目录表\n");
    print_pgdir();
   
    cprintf("\n4. 物理内存管理器\n");
    cprintf("memory management: %s\n", pmm_manager->name);
    
    kmalloc_init();

}
 

//get_pte -获取pte并返回该pte的内核虚拟地址
// -如果PT包含这个pte不存在，为PT分配一个页面
//参数:
// pgdir: PDT的内核虚拟基地地址
// la:需要映射的线性地址
// 创建一个逻辑值来决定是否为PT分配一个页面
// 返回值:这个pte的内核虚拟地址
pte_t *
get_pte(pde_t *pgdir, uintptr_t la, bool create) {
#if 0
    pde_t *pdep = NULL;   // (1) find page directory entry
    if (0) {              // (2) check if entry is not present
                          // (3) check if creating is needed, then alloc page for page table
                          // CAUTION: this page is used for page table, not for common data page
                          // (4) set page reference
        uintptr_t pa = 0; // (5) get linear address of page
                          // (6) clear page content using memset
                          // (7) set page directory entry's permission
    }
    return NULL;          // (8) return page table entry
#endif
    pde_t *pdep = &pgdir[PDX(la)];
    if (!(*pdep & PTE_P)) {
        struct Page *page;
        if (!create || (page = alloc_page()) == NULL) {
            return NULL;
        }
        set_page_ref(page, 1);
        uintptr_t pa = page2pa(page);
        memset(KADDR(pa), 0, PGSIZE);
        *pdep = pa | PTE_U | PTE_W | PTE_P;
    }
    return &((pte_t *)KADDR(PDE_ADDR(*pdep)))[PTX(la)];
}

//get_page -使用PDT pgdir获取线性地址la的相关Page结构struct Page *
struct Page *
get_page(pde_t *pgdir, uintptr_t la, pte_t **ptep_store) {
    pte_t *ptep = get_pte(pgdir, la, 0);
    if (ptep_store != NULL) {
        *ptep_store = ptep;
    }
    if (ptep != NULL && *ptep & PTE_P) {
        return pte2page(*ptep);
    }
    return NULL;
}

//page_remove_pte - 释放一个与线性地址la相关的Page 结构并清除 (无效) pte
//注意:PT被改变了，所以TLB需要失效
static inline void
page_remove_pte(pde_t *pgdir, uintptr_t la, pte_t *ptep) {

#if 0
    if (0) {                      //(1) check if page directory is present
        struct Page *page = NULL; //(2) find corresponding page to pte
                                  //(3) decrease page reference
                                  //(4) and free this page when page reference reachs 0
                                  //(5) clear second page table entry
                                  //(6) flush tlb
    }
#endif
    if (*ptep & PTE_P) {
        struct Page *page = pte2page(*ptep);
        if (page_ref_dec(page) == 0) {
            free_page(page);
        }
        *ptep = 0;
        tlb_invalidate(pgdir, la);
    }
}

void
unmap_range(pde_t *pgdir, uintptr_t start, uintptr_t end) {
    assert(start % PGSIZE == 0 && end % PGSIZE == 0);
    assert(USER_ACCESS(start, end));

    do {
        pte_t *ptep = get_pte(pgdir, start, 0);
        if (ptep == NULL) {
            start = ROUNDDOWN(start + PTSIZE, PTSIZE);
            continue ;
        }
        if (*ptep != 0) {
            page_remove_pte(pgdir, start, ptep);
        }
        start += PGSIZE;
    } while (start != 0 && start < end);
}

void
exit_range(pde_t *pgdir, uintptr_t start, uintptr_t end) {
    assert(start % PGSIZE == 0 && end % PGSIZE == 0);
    assert(USER_ACCESS(start, end));

    start = ROUNDDOWN(start, PTSIZE);
    do {
        int pde_idx = PDX(start);
        if (pgdir[pde_idx] & PTE_P) {
            free_page(pde2page(pgdir[pde_idx]));
            pgdir[pde_idx] = 0;
        }
        start += PTSIZE;
    } while (start != 0 && start < end);
}

/* copy_range - -将一个进程A的内存内容(start，end)复制到另一个进程B
 * @to:    进程B的页面目录的地址
 * @from:  进程A的页面目录的地址
 * @share: 表示dup或share的标志
 *
 * 调用关系: copy_mm-->dup_mmap-->copy_range
 */
int
copy_range(pde_t *to, pde_t *from, uintptr_t start, uintptr_t end, bool share) {
    assert(start % PGSIZE == 0 && end % PGSIZE == 0);
    assert(USER_ACCESS(start, end));
   //按页面单元复制内容。
    do {
        //调用get_pte根据addr开始查找进程A的pte
        pte_t *ptep = get_pte(from, start, 0), *nptep;
        if (ptep == NULL) {
            start = ROUNDDOWN(start + PTSIZE, PTSIZE);
            continue ;
        }
        //调用get_pte根据addr开始查找进程B的pte。如果pte为NULL，就分配一个PT
        if (*ptep & PTE_P) {
            if ((nptep = get_pte(to, start, 1)) == NULL) {
                return -E_NO_MEM;
            }
        uint32_t perm = (*ptep & PTE_USER);
        //从ptep中获取页面
        struct Page *page = pte2page(*ptep);
        //为进程B分配一个页面
        struct Page *npage=alloc_page();
        assert(page!=NULL);
        assert(npage!=NULL);
        int ret=0;

        void * kva_src = page2kva(page);
        void * kva_dst = page2kva(npage);
    
        memcpy(kva_dst, kva_src, PGSIZE);

        ret = page_insert(to, npage, start, perm);
        assert(ret == 0);
        }
        start += PGSIZE;
    } while (start != 0 && start < end);
    return 0;
}

//page_remove - 释放一个与线性地址la相关的页面，并且有一个已验证的pte
void
page_remove(pde_t *pgdir, uintptr_t la) {
    pte_t *ptep = get_pte(pgdir, la, 0);
    if (ptep != NULL) {
        page_remove_pte(pgdir, la, ptep);
    }
}

//page_insert - 用线性地址la建立一个页的物理地址的映射
//  参数：
//  pgdir: PDT的内核虚拟基地址
//  page:  需要映射的页
//  la:    需要映射的线性地址
//  perm:  此页面在相关的pte中设置的权限
//  返回值:总是0
//  注意:PT被改变了，所以TLB需要失效
int
page_insert(pde_t *pgdir, struct Page *page, uintptr_t la, uint32_t perm) {
    pte_t *ptep = get_pte(pgdir, la, 1);
    if (ptep == NULL) {
        return -E_NO_MEM;
    }
    page_ref_inc(page);
    if (*ptep & PTE_P) {
        struct Page *p = pte2page(*ptep);
        if (p == page) {
            page_ref_dec(page);
        }
        else {
            page_remove_pte(pgdir, la, ptep);
        }
    }
    *ptep = page2pa(page) | PTE_P | perm;
    tlb_invalidate(pgdir, la);
    return 0;
}

//使TLB条目无效，但只有在被编辑的页表是当前被处理器使用的页表时才有效。
void
tlb_invalidate(pde_t *pgdir, uintptr_t la) {
    if (rcr3() == PADDR(pgdir)) {
        invlpg((void *)la);
    }
}

// pgdir_alloc_page - 调用alloc_page和page_insert函数
//                  - 分配一个页面大小的内存并设置一个地址映射
//                  - pa<->la
struct Page *
pgdir_alloc_page(pde_t *pgdir, uintptr_t la, uint32_t perm) {
    struct Page *page = alloc_page();
    if (page != NULL) {
        if (page_insert(pgdir, page, la, perm) != 0) {
            free_page(page);
            return NULL;
        }
        if (swap_init_ok){
            if(check_mm_struct!=NULL) {
                swap_map_swappable(check_mm_struct, la, page, 0);
                page->pra_vaddr=la;
                assert(page_ref(page) == 1);
                //cprintf("get No. %d  page: pra_vaddr %x, pra_link.prev %x, pra_link_next %x in pgdir_alloc_page\n", (page-pages), page->pra_vaddr,page->pra_page_link.prev, page->pra_page_link.next);
            } 
            else  {  //now current is existed, should fix it in the future
                //swap_map_swappable(current->mm, la, page, 0);
                //page->pra_vaddr=la;
                //assert(page_ref(page) == 1);
                //panic("pgdir_alloc_page: no pages. now current is existed, should fix it in the future\n");
            }
        }

    }

    return page;
}

void
check_alloc_page(void) {
    pmm_manager->check();
    //cprintf("check_alloc_page() succeeded!\n");
}

void
check_pgdir(void) {
    assert(npage <= KMEMSIZE / PGSIZE);
    assert(boot_pgdir != NULL && (uint32_t)PGOFF(boot_pgdir) == 0);
    assert(get_page(boot_pgdir, 0x0, NULL) == NULL);

    struct Page *p1, *p2;
    p1 = alloc_page();
    assert(page_insert(boot_pgdir, p1, 0x0, 0) == 0);

    pte_t *ptep;
    assert((ptep = get_pte(boot_pgdir, 0x0, 0)) != NULL);
    assert(pte2page(*ptep) == p1);
    //cprintf("page_ref(p1) = %d\n", page_ref(p1));
    //assert(page_ref(p1) == 2);

    ptep = &((pte_t *)KADDR(PDE_ADDR(boot_pgdir[0])))[1];
    assert(get_pte(boot_pgdir, PGSIZE, 0) == ptep);

    p2 = alloc_page();
    assert(page_insert(boot_pgdir, p2, PGSIZE, PTE_U | PTE_W) == 0);
    assert((ptep = get_pte(boot_pgdir, PGSIZE, 0)) != NULL);
    assert(*ptep & PTE_U);
    assert(*ptep & PTE_W);
    assert(boot_pgdir[0] & PTE_U);
    assert(page_ref(p2) == 1);

    assert(page_insert(boot_pgdir, p1, PGSIZE, 0) == 0);
    //assert(page_ref(p1) == 1);
    assert(page_ref(p2) == 0);
    assert((ptep = get_pte(boot_pgdir, PGSIZE, 0)) != NULL);
    assert(pte2page(*ptep) == p1);
    assert((*ptep & PTE_U) == 0);

    page_remove(boot_pgdir, 0x0);
    //assert(page_ref(p1) == 0);
    assert(page_ref(p2) == 0);

    page_remove(boot_pgdir, PGSIZE);
    //assert(page_ref(p1) == 0);
    assert(page_ref(p2) == 0);
    
    

    assert(page_ref(pde2page(boot_pgdir[0])) == 1);
    free_page(pde2page(boot_pgdir[0]));
    boot_pgdir[0] = 0;

    //cprintf("check_pgdir() succeeded!\n");
}

void
check_boot_pgdir(void) {
    pte_t *ptep;
    int i;
    for (i = 0; i < npage; i += PGSIZE) {
        assert((ptep = get_pte(boot_pgdir, (uintptr_t)KADDR(i), 0)) != NULL);
        assert(PTE_ADDR(*ptep) == i);
    }

    assert(PDE_ADDR(boot_pgdir[PDX(VPT)]) == PADDR(boot_pgdir));

    assert(boot_pgdir[0] == 0);

    struct Page *p;
    p = alloc_page();
    assert(page_insert(boot_pgdir, p, 0x100, PTE_W) == 0);
    assert(page_ref(p) == 1);
    assert(page_insert(boot_pgdir, p, 0x100 + PGSIZE, PTE_W) == 0);
    assert(page_ref(p) == 2);

    const char *str = "ucore: Hello world!!";
    strcpy((void *)0x100, str);
    assert(strcmp((void *)0x100, (void *)(0x100 + PGSIZE)) == 0);

    *(char *)(page2kva(p) + 0x100) = '\0';
    assert(strlen((const char *)0x100) == 0);

    free_page(p);
    free_page(pde2page(boot_pgdir[0]));
    boot_pgdir[0] = 0;
    
    tlb_invalidate(boot_pgdir, 0x100);
    tlb_invalidate(boot_pgdir, 0x100+PGSIZE);
    
    //cprintf("check_boot_pgdir() succeeded!\n");
}

//perm2str - -使用字符串'u,r,w，-'表示权限
static const char *
perm2str(int perm) {
    static char str[4];
    str[0] = (perm & PTE_U) ? 'u' : '-';
    str[1] = 'r';
    str[2] = (perm & PTE_W) ? 'w' : '-';
    str[3] = '\0';
    return str;
}

//get_pgtable_items - 在PDT或PT的[left，right]范围内，找到一个连续的线性addr空间
//                  - PDT或PT: (left_store*X_SIZE~right_store*X_SIZE) for PDT or PT
//                  - PDT: X_SIZE=PTSIZE=4M; PT: X_SIZE=PGSIZE=4K
static int
get_pgtable_items(size_t left, size_t right, size_t start, uintptr_t *table, size_t *left_store, size_t *right_store) {
    if (start >= right) {
        return 0;
    }
    while (start < right && !(table[start] & PTE_P)) {
        start ++;
    }
    if (start < right) {
        if (left_store != NULL) {
            *left_store = start;
        }
        int perm = (table[start ++] & PTE_USER);
        while (start < right && (table[start] & PTE_USER) == perm) {
            start ++;
        }
        if (right_store != NULL) {
            *right_store = start;
        }
        return perm;
    }
    return 0;
}

//print_pgdir - print the PDT&PT
void
print_pgdir(void) {
    cprintf("-------------------- BEGIN --------------------\n");
    size_t left, right = 0, perm;
    while ((perm = get_pgtable_items(0, NPDEENTRY, right, vpd, &left, &right)) != 0) {
        cprintf("Page Directory Entry(%03x) %08x-%08x %08x %s\n", right - left,
                left * PTSIZE, right * PTSIZE, (right - left) * PTSIZE, perm2str(perm));
        size_t l, r = left * NPTEENTRY;
        while ((perm = get_pgtable_items(left * NPTEENTRY, right * NPTEENTRY, r, vpt, &l, &r)) != 0) {
            cprintf("  |-- Page Directory Entry(%05x) %08x-%08x %08x %s\n", r - l,
                    l * PGSIZE, r * PGSIZE, (r - l) * PGSIZE, perm2str(perm));
        }
    }
    cprintf("--------------------- END ---------------------\n");
}

