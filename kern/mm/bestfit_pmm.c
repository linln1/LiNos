 #include <pmm.h>
#include <list.h>
#include <string.h>
#include <bestfit_pmm.h>

free_area_t free_area;

#define free_list (free_area.free_list)
#define nr_free (free_area.nr_free)

static void
best_init(void) {
    list_init(&free_list);
    nr_free = 0;
}

static void
best_init_memmap(struct Page *base, size_t n) {
    assert(n > 0);
    struct Page *p = base;
    for (; p != base + n; p ++) {
        assert(PageReserved(p));
        p->flags = p->property = 0;
        set_page_ref(p, 0);
    }
    base->property = n;
    SetPageProperty(base);
    nr_free += n;
    list_add_before(&free_list, &(base->page_link));
}

static struct Page *
best_alloc_pages(size_t n) {
    assert(n > 0);
    if (n > nr_free) {
        return NULL;
    }
    list_entry_t *le=&free_list;
    struct Page *best_p = NULL;
    int best_property=-1,flag=0;    //best_property用于标识是不是第一次找到满足条件的块
    while((le=list_next(le))!=&free_list){  // 遍历所有空闲块，找到满足需求（>n）且最小的块（即找到最优的块）
     struct Page *p = le2page(le,page_link);
     if(p->property>=n){
	if(best_property==-1||p->property<best_property){
                best_p=p;
		best_property=p->property;
		flag=1; // 说明此时有临时的最优块
	}
     }
   }
	if(flag==0)
	  return NULL;
        if (best_p != NULL) 
	{
        if (best_p->property > n) 
        {
            struct Page *p = best_p + n;
            p->property = best_p->property - n;
            SetPageProperty(p);
            list_add_after(&free_list, &(p->page_link));
        }
	    list_del(&(best_p->page_link));
        nr_free -= n;
        ClearPageProperty(best_p);
    }
    return best_p;
    
}

static void
best_free_pages(struct Page *base, size_t n) {
    assert(n > 0);
    struct Page *p = base;
    for (; p != base + n; p ++) {
        assert(!PageReserved(p) && !PageProperty(p));
        p->flags = 0;
        set_page_ref(p, 0);
    }
    base->property = n;
    SetPageProperty(base);
    list_entry_t *le = list_next(&free_list);
    while (le != &free_list) {
        p = le2page(le, page_link);
        le = list_next(le);
        if (base + base->property == p) {
            base->property += p->property;
            ClearPageProperty(p);
            list_del(&(p->page_link));
        }
        else if (p + p->property == base) {
            p->property += base->property;
            ClearPageProperty(base);
            base = p;
            list_del(&(p->page_link));
        }
    }
    nr_free += n;
    le = list_next(&free_list);
    while (le != &free_list) {
        p = le2page(le, page_link);
        if (base + base->property <= p) {
            assert(base + base->property != p);
            break;
        }
        le = list_next(le);
    }
    list_add_before(le, &(base->page_link));
}



static size_t
best_nr_free_pages(void) {
    return nr_free;
}

static void
basic_check(void) {
    struct Page *p0, *p1, *p2;
    p0 = p1 = p2 = NULL;
    assert((p0 = alloc_page()) != NULL);
    assert((p1 = alloc_page()) != NULL);
    assert((p2 = alloc_page()) != NULL);

    assert(p0 != p1 && p0 != p2 && p1 != p2);
    assert(page_ref(p0) == 0 && page_ref(p1) == 0 && page_ref(p2) == 0);

    assert(page2pa(p0) < npage * PGSIZE);
    assert(page2pa(p1) < npage * PGSIZE);
    assert(page2pa(p2) < npage * PGSIZE);

    list_entry_t free_list_store = free_list;
    list_init(&free_list);
    assert(list_empty(&free_list));

    unsigned int nr_free_store = nr_free;
    nr_free = 0;

    assert(alloc_page() == NULL);

    free_page(p0);
    free_page(p1);
    free_page(p2);
    assert(nr_free == 3);

    assert((p0 = alloc_page()) != NULL);
    assert((p1 = alloc_page()) != NULL);
    assert((p2 = alloc_page()) != NULL);

    assert(alloc_page() == NULL);

    free_page(p0);
    assert(!list_empty(&free_list));

    struct Page *p;
    assert((p = alloc_page()) == p0);
    assert(alloc_page() == NULL);

    assert(nr_free == 0);
    free_list = free_list_store;
    nr_free = nr_free_store;

    free_page(p);
    free_page(p1);
    free_page(p2);
}


static void
best_check(void) {
    int count = 0, total = 0;
    list_entry_t *le = &free_list;
    while ((le = list_next(le)) != &free_list) {
        struct Page *p = le2page(le, page_link);
        assert(PageProperty(p));
        count ++, total += p->property;
    }
    assert(total == nr_free_pages());

    basic_check();

    struct Page *p0 = alloc_pages(5), *p1, *p2;
    assert(p0 != NULL);
    assert(!PageProperty(p0));

    list_entry_t free_list_store = free_list;
    list_init(&free_list);
    assert(list_empty(&free_list));
    assert(alloc_page() == NULL);

    unsigned int nr_free_store = nr_free;
    nr_free = 0;

    free_pages(p0 + 2, 3);
    assert(alloc_pages(4) == NULL);
    assert(PageProperty(p0 + 2) && p0[2].property == 3);
    assert((p1 = alloc_pages(3)) != NULL);
    assert(alloc_page() == NULL);
    assert(p0 + 2 == p1);

    p2 = p0 + 1;
    free_page(p0);
    free_pages(p1, 3);
    assert(PageProperty(p0) && p0->property == 1);
    assert(PageProperty(p1) && p1->property == 3);

    assert((p0 = alloc_page()) == p2 - 1);
    free_page(p0);
    assert((p0 = alloc_pages(2)) == p2 + 1);

    free_pages(p0, 2);
    free_page(p2);

    assert((p0 = alloc_pages(5)) != NULL);
    assert(alloc_page() == NULL);

    assert(nr_free == 0);
    nr_free = nr_free_store;

    free_list = free_list_store;
    free_pages(p0, 5);

    le = &free_list;
    while ((le = list_next(le)) != &free_list) {
        struct Page *p = le2page(le, page_link);
        count --, total -= p->property;
    }
    assert(count == 0);
    assert(total == 0);
}

static void
bestfit_user_check(void){
    struct Page *p0, *p1, *p2;
    p0 = p1 = p2 = NULL;
    assert((p0 = alloc_page()) != NULL);
    cprintf("assert((p0 = alloc_page()) != NULL)\n");
    assert((p1 = alloc_page()) != NULL);	
    cprintf("assert((p1 = alloc_page()) != NULL)\n");
    assert((p2 = alloc_page()) != NULL);
    cprintf("assert((p2 = alloc_page()) != NULL)\n");

    assert(p0 != p1 && p0 != p2 && p1 != p2);
    cprintf("assert(p0 != p1 && p0 != p2 && p1 != p2)\n");
    assert(page_ref(p0) == 0 && page_ref(p1) == 0 && page_ref(p2) == 0);
    cprintf("assert(page_ref(p0) == 0 && page_ref(p1) == 0 && page_ref(p2) == 0)\n");


    assert(page2pa(p0) < npage * PGSIZE);
    cprintf("assert(page2pa(p0) < npage * PGSIZE)\n");
    assert(page2pa(p1) < npage * PGSIZE);
    cprintf("assert(page2pa(p01) < npage * PGSIZE)\n");
    assert(page2pa(p2) < npage * PGSIZE);
    cprintf("assert(page2pa(p2) < npage * PGSIZE)\n");

    list_entry_t free_list_store = free_list;
    list_init(&free_list);
    assert(list_empty(&free_list));

    unsigned int nr_free_store = nr_free;
    nr_free = 0;
    cprintf("assert(alloc_page() != NULL) because of swap out! \n");

    free_page(p0);
    cprintf("free_page(p0)\n");
    free_page(p1);
    cprintf("free_page(p1)\n");
    free_page(p2);
    cprintf("free_page(p2)\n");
    assert(nr_free == 3);
    cprintf("assert(nr_free == 3)\n");

}

const struct pmm_manager bestfit_pmm_manager = {
    .name = "bestfit_pmm_manager",
    .init = best_init,
    .init_memmap = best_init_memmap,
    .alloc_pages = best_alloc_pages,
    .free_pages = best_free_pages,
    .nr_free_pages = best_nr_free_pages,
    .check = best_check,
    .user_check = bestfit_user_check,
};
