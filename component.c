    //看一下虚拟地址对应的物理地址
    while((le = list_next(le)) != &(mm->mmap_list)){
        struct vma_struct *temp_vma = le2vma(le, list_link);
        uintptr_t start_addr = temp_vma->vm_start;
        uintptr_t end_addr = start_addr + PGSIZE;
	uintptr_t end_up = ROUNDUP(temp_vma->vm_end, PGSIZE);
	
	for(;end_addr <= end_up ; start_addr+=PGSIZE, end_addr+=PGSIZE){
	        pte_t *pes = get_pte(mm->pgdir ,start_addr, 0);
        
		//PDX(start_addr) 该地址在页目录表中的索引， &pgdir[PDX(start_addr)] 页表地址

		uintptr_t phaddr = (*pes & ~0xFFF)  + (start_addr & 0xFFF);

		//前20位对应物理块号，怎么找物理地址

		assert(pes != NULL);
		cprintf("virtual address [ 0x%x - 0x%x ] ----> physical address [ 0x%x ] \n", start_addr, end_addr, phaddr);
	}

    }











    cprintf("================================================\n");
    cprintf("after allocated!\n");
    cprintf("================================================\n");
    while((le = list_next(le)) != &(mm->mmap_list)){
        struct vma_struct *temp_vma = le2vma(le, list_link);
        uintptr_t start_addr = temp_vma->vm_start;
        uintptr_t end_addr = start_addr + PGSIZE;
	uintptr_t end_up = ROUNDUP(temp_vma->vm_end, PGSIZE);
	
	for(;end_addr <= end_up ; start_addr+=PGSIZE, end_addr+=PGSIZE){
	        pte_t *pes = get_pte(mm->pgdir ,start_addr, 0);
        
		//PDX(start_addr) 该地址在页目录表中的索引， &pgdir[PDX(start_addr)] 页表地址

		uintptr_t phaddr = (*pes & ~0xFFF)  + (start_addr & 0xFFF);

		//前20位对应物理块号，怎么找物理地址

		assert(pes != NULL);
		cprintf("virtual address [ 0x%x - 0x%x ] ----> physical address [ 0x%x ] \n", start_addr, end_addr, phaddr);
	}

    }
