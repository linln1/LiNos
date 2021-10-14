#ifndef __KERN_MM_PMM_H__
#define __KERN_MM_PMM_H__

#include <defs.h>
#include <mmu.h>
#include <memlayout.h>
#include <atomic.h>
#include <assert.h>

//pmm_manager是一个物理内存管理类。一个特殊的pmm管理器—XXX_pmm_manager
//只需要实现pmm_manager类中的方法，就可以使用XXX_pmm_manager
//通过ucore管理总物理内存空间。
struct pmm_manager {
    const char *name;                                 // XXX_pmm_manager的名字
    void (*init)(void);                               //初始化内部描述和管理数据结构
                                                      // (空闲的块列表,XXX_pmm_manager空闲块)
    void (*init_memmap)(struct Page *base, size_t n); //设置描述和管理数据结构
                                                      // 最初的可用物理内存空间
    struct Page *(*alloc_pages)(size_t n);            // 分配>=n个页面，取决于分配算法
    void (*free_pages)(struct Page *base, size_t n);  // 释放>=n个具有“base”addr的页面(memlayout.h)
    size_t (*nr_free_pages)(void);                    // 返回空闲页面的数量
    void (*check)(void);                              // 检查XXX_pmm_manager的正确性
    void (*user_check)(void);		 	      // 用户检查pmm_manager的正确性
};

extern const struct pmm_manager *pmm_manager;
extern pde_t *boot_pgdir;
extern uintptr_t boot_cr3;

void pmm_init(void);

struct Page *alloc_pages(size_t n);
void free_pages(struct Page *base, size_t n);
void user_check(void);
size_t nr_free_pages(void);

#define alloc_page() alloc_pages(1)
#define free_page(page) free_pages(page, 1)

pte_t *get_pte(pde_t *pgdir, uintptr_t la, bool create);
struct Page *get_page(pde_t *pgdir, uintptr_t la, pte_t **ptep_store);
void page_remove(pde_t *pgdir, uintptr_t la);
int page_insert(pde_t *pgdir, struct Page *page, uintptr_t la, uint32_t perm);

void load_esp0(uintptr_t esp0);
void tlb_invalidate(pde_t *pgdir, uintptr_t la);
struct Page *pgdir_alloc_page(pde_t *pgdir, uintptr_t la, uint32_t perm);
void unmap_range(pde_t *pgdir, uintptr_t start, uintptr_t end);
void exit_range(pde_t *pgdir, uintptr_t start, uintptr_t end);
int copy_range(pde_t *to, pde_t *from, uintptr_t start, uintptr_t end, bool share);

void print_pgdir(void);

void check_alloc_page();
void check_pgdir();
void check_boot_pgdir();

/* *
* PADDR -接受内核虚拟地址(一个指向KERNBASE之上的地址)，
*其中映射了机器的最大256MB物理内存，并返回
*对应的物理地址。如果传递给它一个非内核虚拟地址，它就会panic。
* */
#define PADDR(kva) ({                                                   \
            uintptr_t __m_kva = (uintptr_t)(kva);                       \
            if (__m_kva < KERNBASE) {                                   \
                panic("PADDR called with invalid kva %08lx", __m_kva);  \
            }                                                           \
            __m_kva - KERNBASE;                                         \
        })

/* *
* KADDR -获取一个物理地址并返回相应的内核虚拟地址
*地址。如果传递一个无效的物理地址，它就会panic
* */
#define KADDR(pa) ({                                                    \
            uintptr_t __m_pa = (pa);                                    \
            size_t __m_ppn = PPN(__m_pa);                               \
            if (__m_ppn >= npage) {                                     \
                panic("KADDR called with invalid pa %08lx", __m_pa);    \
            }                                                           \
            (void *) (__m_pa + KERNBASE);                               \
        })

extern struct Page *pages;
extern size_t npage;

static inline ppn_t
page2ppn(struct Page *page) {
    return page - pages;
}

static inline uintptr_t
page2pa(struct Page *page) {
    return page2ppn(page) << PGSHIFT;
}

static inline struct Page *
pa2page(uintptr_t pa) {
    if (PPN(pa) >= npage) {
        panic("pa2page called with invalid pa");
    }
    return &pages[PPN(pa)];
}

static inline void *
page2kva(struct Page *page) {
    return KADDR(page2pa(page));
}

static inline struct Page *
kva2page(void *kva) {
    return pa2page(PADDR(kva));
}

static inline struct Page *
pte2page(pte_t pte) {
    if (!(pte & PTE_P)) {
        panic("pte2page called with invalid pte");
    }
    return pa2page(PTE_ADDR(pte));
}

static inline struct Page *
pde2page(pde_t pde) {
    return pa2page(PDE_ADDR(pde));
}

static inline int
page_ref(struct Page *page) {
    return page->ref;
}

static inline void
set_page_ref(struct Page *page, int val) {
    page->ref = val;
}

static inline int
page_ref_inc(struct Page *page) {
    page->ref += 1;
    return page->ref;
}

static inline int
page_ref_dec(struct Page *page) {
    page->ref -= 1;
    return page->ref;
}

extern char bootstack[], bootstacktop[];

#endif /* !__KERN_MM_PMM_H__ */

