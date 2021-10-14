#include <proc.h>
#include <kmalloc.h>
#include <string.h>
#include <sync.h>
#include <pmm.h>
#include <error.h>
#include <sched.h>
#include <elf.h>
#include <vmm.h>
#include <trap.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <fs.h>
#include <vfs.h>
#include <sysfile.h>

/* ------------- process/thread mechanism design&implementation -------------
(an simplified Linux process/thread mechanism )
introduction:
  ucore implements a simple process/thread mechanism. process contains the independent memory sapce, at least one threads
for execution, the kernel data(for management), processor state (for context switch), files(in lab6), etc. ucore needs to
manage all these details efficiently. In ucore, a thread is just a special kind of process(share process's memory).
------------------------------
process state       :     meaning               -- reason
    PROC_UNINIT     :   uninitialized           -- alloc_proc
    PROC_SLEEPING   :   sleeping                -- try_free_pages, do_wait, do_sleep
    PROC_RUNNABLE   :   runnable(maybe running) -- proc_init, wakeup_proc, 
    PROC_ZOMBIE     :   almost dead             -- do_exit

-----------------------------
process state changing:
                                            
  alloc_proc                                 RUNNING
      +                                   +--<----<--+
      +                                   + proc_run +
      V                                   +-->---->--+ 
PROC_UNINIT -- proc_init/wakeup_proc --> PROC_RUNNABLE -- try_free_pages/do_wait/do_sleep --> PROC_SLEEPING --
                                           A      +                                                           +
                                           |      +--- do_exit --> PROC_ZOMBIE                                +
                                           +                                                                  + 
                                           -----------------------wakeup_proc----------------------------------
-----------------------------
process relations
parent:           proc->parent  (proc is children)
children:         proc->cptr    (proc is parent)
older sibling:    proc->optr    (proc is younger sibling)
younger sibling:  proc->yptr    (proc is older sibling)
-----------------------------
related syscall for process:
SYS_exit        : process exit,                           -->do_exit
SYS_fork        : create child process, dup mm            -->do_fork-->wakeup_proc
SYS_wait        : wait process                            -->do_wait
SYS_exec        : after fork, process execute a program   -->load a program and refresh the mm
SYS_clone       : create child thread                     -->do_fork-->wakeup_proc
SYS_yield       : process flag itself need resecheduling, -- proc->need_sched=1, then scheduler will rescheule this process
SYS_sleep       : process sleep                           -->do_sleep 
SYS_kill        : kill process                            -->do_kill-->proc->flags |= PF_EXITING
                                                                 -->wakeup_proc-->do_wait-->do_exit   
SYS_getpid      : get the process's pid

*/

// the process set's list
list_entry_t proc_list;

#define HASH_SHIFT          10
#define HASH_LIST_SIZE      (1 << HASH_SHIFT)
#define pid_hashfn(x)       (hash32(x, HASH_SHIFT))

// has list for process set based on pid
// 根据pid进行组织的hash list数组
static list_entry_t hash_list[HASH_LIST_SIZE];

// idle proc
struct proc_struct *idleproc = NULL;
// init proc
struct proc_struct *initproc = NULL;
// current proc
struct proc_struct *current = NULL;

static int nr_process = 0;//初始化进程数为零

void kernel_thread_entry(void);
void forkrets(struct trapframe *tf);
void switch_to(struct context *from, struct context *to);

// alloc_proc - alloc a proc_struct and init all fields of proc_struct
static struct proc_struct *
alloc_proc(void) {

	//
	/*
	 *    在进程控制之快中的下面这些变量
	 *       enum proc_state state;                      // 进程状态
	 *       int pid;                                    // 进程ID
	 *       int runs;                                   // 进程运行时间
	 *       uintptr_t kstack;                           // 进程内核栈
	 *       volatile bool need_resched;                 // 标志位，用于表示可以被调度释放CPU资源
	 *       struct proc_struct *parent;                 //进程的父进程
	 *       struct mm_struct *mm;                       // 进程可用空间
	 *       struct context context;                     //进程上下文，用于切换进程
	 *       struct trapframe *tf;                       // 中断帧，用于进程中断
	 *       uintptr_t cr3;                              // 页地址寄存器
	 *       uint32_t flags;                             // 进程标志
	 *       char name[PROC_NAME_LEN + 1];               // 进程名称
	 */
	 //
    struct proc_struct *proc = kmalloc(sizeof(struct proc_struct));
    if (proc != NULL) {
		//初始化
        proc->state = PROC_UNINIT;          
        proc->pid = -1;
        proc->runs = 0;
        proc->kstack = 0;
        proc->need_resched = 0;
        proc->parent = NULL;
        proc->mm = NULL;
        memset(&(proc->context), 0, sizeof(struct context));
        proc->tf = NULL;
		// boot_cr3是内核态进程CR3地址，如果是用户态需要相应修改
        proc->cr3 = boot_cr3;
        proc->flags = 0;
        memset(proc->name, 0, PROC_NAME_LEN);
        proc->wait_state = 0;
        proc->cptr = proc->optr = proc->yptr = NULL;
        proc->rq = NULL;
        list_init(&(proc->run_link));
        proc->time_slice = 0;
        proc->lab6_run_pool.left = proc->lab6_run_pool.right = proc->lab6_run_pool.parent = NULL;
        proc->stride = 0;
        proc->priority = 0;
        proc->filesp = NULL;
    }
    return proc;
}

// 用于设置进程名称
char *
set_proc_name(struct proc_struct *proc, const char *name) {
    memset(proc->name, 0, sizeof(proc->name));
    return memcpy(proc->name, name, PROC_NAME_LEN);
}

// get_proc_name - 获取进程名称
char *
get_proc_name(struct proc_struct *proc) {
    static char name[PROC_NAME_LEN + 1];
    memset(name, 0, sizeof(name));
    return memcpy(name, proc->name, PROC_NAME_LEN);
}

// set_links - 设置进程关系链表
static void
set_links(struct proc_struct *proc) {
    list_add(&proc_list, &(proc->list_link));
    proc->yptr = NULL;
    if ((proc->optr = proc->parent->cptr) != NULL) {
        proc->optr->yptr = proc;
    }
    proc->parent->cptr = proc;
    nr_process ++;
}

// remove_links - 删除进程间的关系
static void
remove_links(struct proc_struct *proc) {
    list_del(&(proc->list_link));
    if (proc->optr != NULL) {
        proc->optr->yptr = proc->yptr;
    }
    if (proc->yptr != NULL) {
        proc->yptr->optr = proc->optr;
    }
    else {
       proc->parent->cptr = proc->optr;
    }
    nr_process --;
}

// get_pid - 为进程申请一个单独的pid
//分配进程系统唯一的pid
static int
get_pid(void) {
    static_assert(MAX_PID > MAX_PROCESS);
    struct proc_struct *proc;
    list_entry_t *list = &proc_list, *le;
    static int next_safe = MAX_PID, last_pid = MAX_PID;
    if (++ last_pid >= MAX_PID) {
        last_pid = 1;
        goto inside;
    }
    if (last_pid >= next_safe) {
    inside:
        next_safe = MAX_PID;
    repeat:
        le = list;
        while ((le = list_next(le)) != list) {
            proc = le2proc(le, list_link);
            if (proc->pid == last_pid) {
                if (++ last_pid >= next_safe) {
                    if (last_pid >= MAX_PID) {
                        last_pid = 1;
                    }
                    next_safe = MAX_PID;
                    goto repeat;
                }
            }
            else if (proc->pid > last_pid && next_safe > proc->pid) {
                next_safe = proc->pid;
            }
        }
    }
    return last_pid;
}

// proc_run - 是进程在CPU上运行
void
proc_run(struct proc_struct *proc) {
    if (proc != current) {
        bool intr_flag;
        struct proc_struct *prev = current, *next = proc;
        local_intr_save(intr_flag);
        {
            current = proc;
            load_esp0(next->kstack + KSTACKSIZE);
            lcr3(next->cr3);
            switch_to(&(prev->context), &(next->context));
        }
        local_intr_restore(intr_flag);
    }
}

// forkret -- the first kernel entry point of a new thread/process
static void
forkret(void) {
    forkrets(current->tf);
}

// hash_proc - add proc into proc hash_list
static void
hash_proc(struct proc_struct *proc) {
    list_add(hash_list + pid_hashfn(proc->pid), &(proc->hash_link));
}

// unhash_proc - delete proc from proc hash_list
static void
unhash_proc(struct proc_struct *proc) {
    list_del(&(proc->hash_link));
}

// find_proc - find proc frome proc hash_list according to pid
// 根据进程pid查找进程结构体，其实就是在hash list数组中查找
struct proc_struct *
find_proc(int pid) {
    if (0 < pid && pid < MAX_PID) {     //遍历
        list_entry_t *list = hash_list + pid_hashfn(pid), *le = list;
        while ((le = list_next(le)) != list) {
            struct proc_struct *proc = le2proc(le, hash_link);
            if (proc->pid == pid) {
                return proc;    //查找并找到
            }
        }
    }
    return NULL;
}

// kernel_thread - create a kernel thread using "fn" function
// NOTE: the contents of temp trapframe tf will be copied to 
//       proc->tf in do_fork-->copy_thread function
//创建内核线程
int
kernel_thread(int (*fn)(void *), void *arg, uint32_t clone_flags) {
    struct trapframe tf;
	// 将tf数据结构清零
    memset(&tf, 0, sizeof(struct trapframe));
	// 代码段指向内核代码段
    tf.tf_cs = KERNEL_CS;
	// 数据段指向内核数据段
    tf.tf_ds = tf.tf_es = tf.tf_ss = KERNEL_DS;
	// ebx寄存器用于保存执行函数指针
    tf.tf_regs.reg_ebx = (uint32_t)fn;
	// edx寄存器用于保存参数指针
    tf.tf_regs.reg_edx = (uint32_t)arg;
	// eip寄存器指向kernel_thread_entry函数
    tf.tf_eip = (uint32_t)kernel_thread_entry;
    return do_fork(clone_flags | CLONE_VM, 0, &tf);
}

// setup_kstack - alloc pages with size KSTACKPAGE as process kernel stack
// 分配内核态进程的栈
static int
setup_kstack(struct proc_struct *proc) {
    struct Page *page = alloc_pages(KSTACKPAGE);
    if (page != NULL) {
        proc->kstack = (uintptr_t)page2kva(page);
        return 0;
    }
    return -E_NO_MEM;
}

// put_kstack - free the memory space of process kernel stack
static void
put_kstack(struct proc_struct *proc) {
    free_pages(kva2page((void *)(proc->kstack)), KSTACKPAGE);
}

// setup_pgdir - alloc one page as PDT
// 分配mm数据结构的PDT
static int
setup_pgdir(struct mm_struct *mm) {
    struct Page *page;
	// 分配一个page
    if ((page = alloc_page()) == NULL) {
        return -E_NO_MEM;
    }
	// 得到对应的PDE指针
    pde_t *pgdir = page2kva(page);
	// 将内核的页目录数据拷贝过来
    memcpy(pgdir, boot_pgdir, PGSIZE);
    pgdir[PDX(VPT)] = PADDR(pgdir) | PTE_P | PTE_W;
    mm->pgdir = pgdir;
	// 保存，返回
    return 0;
}

// put_pgdir - free the memory space of PDT
static void
put_pgdir(struct mm_struct *mm) {
    free_page(kva2page(mm->pgdir));
}

// copy_mm - process "proc" duplicate OR share process "current"'s mm according clone_flags
//         - if clone_flags & CLONE_VM, then "share" ; else "duplicate"
static int
copy_mm(uint32_t clone_flags, struct proc_struct *proc) {
    struct mm_struct *mm, *oldmm = current->mm;

    /* current is a kernel thread */
	// 内核态进程mm指针为NULL，此时不需要做拷贝直接返回
    if (oldmm == NULL) {
        return 0;
    }
	// 这个标志位表示父子进程之间共享VM，此时不需要继续做拷贝工作直接返回
    if (clone_flags & CLONE_VM) {
        mm = oldmm;
        goto good_mm;
    }
	// 到了这里说明要给新的进程分配内存空间，把返回值先置为-E_NO_MEM
    int ret = -E_NO_MEM;
    if ((mm = mm_create()) == NULL) {
        goto bad_mm;
    }
    if (setup_pgdir(mm) != 0) {
        goto bad_pgdir_cleanup_mm;
    }

    lock_mm(oldmm);
    {
        ret = dup_mmap(mm, oldmm);
    }
    unlock_mm(oldmm);

    if (ret != 0) {
        goto bad_dup_cleanup_mmap;
    }

good_mm:
    mm_count_inc(mm);
    proc->mm = mm;
	// 到了这里将CR3置为这个进程的页目录指针
    proc->cr3 = PADDR(mm->pgdir);
    return 0;
bad_dup_cleanup_mmap:
    exit_mmap(mm);
    put_pgdir(mm);
bad_pgdir_cleanup_mm:
    mm_destroy(mm);
bad_mm:
    return ret;
}

// copy_thread - setup the trapframe on the  process's kernel stack top and
//             - setup the kernel entry point and stack of process
static void
copy_thread(struct proc_struct *proc, uintptr_t esp, struct trapframe *tf) {
	// 在内核堆栈顶部分配空间做为进程的tf空间
	proc->tf = (struct trapframe *)(proc->kstack + KSTACKSIZE) - 1;
	// 保存传入的tf数据
    *(proc->tf) = *tf;
	// 子进程执行完do_fork函数的返回值
    proc->tf->tf_regs.reg_eax = 0;
    proc->tf->tf_esp = esp;
	// 可以被中断
    proc->tf->tf_eflags |= FL_IF;
	// eip指针用于保存上次停止执行时的下一条指令地址
    proc->context.eip = (uintptr_t)forkret;
	// esp用于保存停止执行的堆栈地址，因为tf这里指向了内核堆栈的起始位置
    proc->context.esp = (uintptr_t)(proc->tf);
}


static int
copy_fs(uint32_t clone_flags, struct proc_struct *proc) {
    struct files_struct *filesp, *old_filesp = current->filesp;
    assert(old_filesp != NULL);

    if (clone_flags & CLONE_FS) {
        filesp = old_filesp;
        goto good_files_struct;
    }

    int ret = -E_NO_MEM;
    if ((filesp = files_create()) == NULL) {
        goto bad_files_struct;
    }

    if ((ret = dup_files(filesp, old_filesp)) != 0) {
        goto bad_dup_cleanup_fs;
    }

good_files_struct:
    files_count_inc(filesp);
    proc->filesp = filesp;
    return 0;

bad_dup_cleanup_fs:
    files_destroy(filesp);
bad_files_struct:
    return ret;
}

static void
put_fs(struct proc_struct *proc) {
    struct files_struct *filesp = proc->filesp;
    if (filesp != NULL) {
        if (files_count_dec(filesp) == 0) {
            files_destroy(filesp);
        }
    }
}

/* do_fork -     parent process for a new child process
 * @clone_flags: used to guide how to clone the child process
 * @stack:       the parent's user stack pointer. if stack==0, It means to fork a kernel thread.
 * @tf:          the trapframe info, which will be copied to child process's proc->tf
 */
int
do_fork(uint32_t clone_flags, uintptr_t stack, struct trapframe *tf) {
    int ret = -E_NO_FREE_PROC;
    struct proc_struct *proc;
	// 首先判断进程数量
    if (nr_process >= MAX_PROCESS) {
        goto fork_out;
    }
    ret = -E_NO_MEM;


    //    1. call alloc_proc to allocate a proc_struct
    //    2. call setup_kstack to allocate a kernel stack for child process
    //    3. call copy_mm to dup OR share mm according clone_flag
    //    4. call copy_thread to setup tf & context in proc_struct
    //    5. insert proc_struct into hash_list && proc_list
    //    6. call wakeup_proc to make the new child process RUNNABLE
    //    7. set ret vaule using child proc's pid

	  // 分配proc结构体指针
    if ((proc = alloc_proc()) == NULL) {
        goto fork_out;
    }

    proc->parent = current;
    assert(current->wait_state == 0);
	// 创建内核栈
    if (setup_kstack(proc) != 0) {
        goto bad_fork_cleanup_proc;
    }
    if (copy_fs(clone_flags, proc) != 0) { //for LAB8
        goto bad_fork_cleanup_kstack;
    }
	// 拷贝内存
    if (copy_mm(clone_flags, proc) != 0) {
        goto bad_fork_cleanup_fs;
    }
    copy_thread(proc, stack, tf);

    bool intr_flag;
    local_intr_save(intr_flag);
    {
        proc->pid = get_pid();//申请pid
        hash_proc(proc);
        set_links(proc);

    }
    local_intr_restore(intr_flag);

    char* std[4]={"PROC_UNINIT","PROC_SLEEPING","PROC_RUNNABLE","PROC_ZOMBIE"};
    cprintf("create proc ... \n");

    cprintf("pid:%d    state:%s    name:%s    time_slice:%d    runtime:%d\n",
        proc->pid, std[proc->state],proc->name,proc->time_slice,proc->runs);
	//打印一些信息
    wakeup_proc(proc);

    ret = proc->pid;
fork_out:
    return ret;

bad_fork_cleanup_fs:  //for LAB8
    put_fs(proc);
bad_fork_cleanup_kstack:
    put_kstack(proc);
bad_fork_cleanup_proc:
    kfree(proc);
    goto fork_out;
}

// do_exit - called by sys_exit
//   1. call exit_mmap & put_pgdir & mm_destroy to free the almost all memory space of process
//   2. set process' state as PROC_ZOMBIE, then call wakeup_proc(parent) to ask parent reclaim itself.
//   3. call scheduler to switch to other process
int
do_exit(int error_code) {
    if (current == idleproc) {
        panic("idleproc exit.\n");
    }
    if (current == initproc) {
        panic("initproc exit.\n");
    }
    
    struct mm_struct *mm = current->mm;
    if (mm != NULL) {
        lcr3(boot_cr3);
        if (mm_count_dec(mm) == 0) {
            exit_mmap(mm);
            put_pgdir(mm);
            mm_destroy(mm);
        }
        current->mm = NULL;
    }
    put_fs(current); 
    current->state = PROC_ZOMBIE;  //将进程状态改为PROC_ZOMBIE
    current->exit_code = error_code;
    
    bool intr_flag;
    struct proc_struct *proc;
    local_intr_save(intr_flag);
    {
        proc = current->parent;
        if (proc->wait_state == WT_CHILD) {
            wakeup_proc(proc);
        }
        while (current->cptr != NULL) {
            proc = current->cptr;
            current->cptr = proc->optr;
    
            proc->yptr = NULL;
            if ((proc->optr = initproc->cptr) != NULL) {
                initproc->cptr->yptr = proc;
            }
            proc->parent = initproc;
            initproc->cptr = proc;
            if (proc->state == PROC_ZOMBIE) {
                if (initproc->wait_state == WT_CHILD) {
                    wakeup_proc(initproc);
                }
            }
        }
    }
    local_intr_restore(intr_flag);
    
    schedule();
    panic("do_exit will not return!! %d.\n", current->pid);
}

//load_icode_read is used by load_icode in LAB8
static int
load_icode_read(int fd, void *buf, size_t len, off_t offset) {
    int ret;
    if ((ret = sysfile_seek(fd, offset, LSEEK_SET)) != 0) {
        return ret;
    }
    if ((ret = sysfile_read(fd, buf, len)) != len) {
        return (ret < 0) ? ret : -1;
    }
    return 0;
}

// load_icode -  called by sys_exec-->do_execve
  
static int
load_icode(int fd, int argc, char **kargv) {

	/* (1) create a new mm for current process
     * (2) create a new PDT, and mm->pgdir= kernel virtual addr of PDT
     * (3) copy TEXT/DATA/BSS parts in binary to memory space of process
     *    (3.1) read raw data content in file and resolve elfhdr
     *    (3.2) read raw data content in file and resolve proghdr based on info in elfhdr
     *    (3.3) call mm_map to build vma related to TEXT/DATA
     *    (3.4) callpgdir_alloc_page to allocate page for TEXT/DATA, read contents in file
     *          and copy them into the new allocated pages
     *    (3.5) callpgdir_alloc_page to allocate pages for BSS, memset zero in these pages
     * (4) call mm_map to setup user stack, and put parameters into user stack
     * (5) setup current process's mm, cr3, reset pgidr (using lcr3 MARCO)
     * (6) setup uargc and uargv in user stacks
     * (7) setup trapframe for user environment
     * (8) if up steps failed, you should cleanup the env.
     */
    assert(argc >= 0 && argc <= EXEC_MAX_ARG_NUM);

    if (current->mm != NULL) {
        panic("load_icode: current->mm must be empty.\n");
    }

    int ret = -E_NO_MEM;
    struct mm_struct *mm;
    if ((mm = mm_create()) == NULL) {
        goto bad_mm;
    }
    if (setup_pgdir(mm) != 0) {
        goto bad_pgdir_cleanup_mm;
    }

    struct Page *page;

    struct elfhdr __elf, *elf = &__elf;
    if ((ret = load_icode_read(fd, elf, sizeof(struct elfhdr), 0)) != 0) {
        goto bad_elf_cleanup_pgdir;
    }

    if (elf->e_magic != ELF_MAGIC) {
        ret = -E_INVAL_ELF;
        goto bad_elf_cleanup_pgdir;
    }

    struct proghdr __ph, *ph = &__ph;
    uint32_t vm_flags, perm, phnum;
    for (phnum = 0; phnum < elf->e_phnum; phnum ++) {
        off_t phoff = elf->e_phoff + sizeof(struct proghdr) * phnum;
        if ((ret = load_icode_read(fd, ph, sizeof(struct proghdr), phoff)) != 0) {
            goto bad_cleanup_mmap;
        }
        if (ph->p_type != ELF_PT_LOAD) {
            continue ;
        }
        if (ph->p_filesz > ph->p_memsz) {
            ret = -E_INVAL_ELF;
            goto bad_cleanup_mmap;
        }
        if (ph->p_filesz == 0) {
            continue ;
        }
        vm_flags = 0, perm = PTE_U;
        if (ph->p_flags & ELF_PF_X) vm_flags |= VM_EXEC;
        if (ph->p_flags & ELF_PF_W) vm_flags |= VM_WRITE;
        if (ph->p_flags & ELF_PF_R) vm_flags |= VM_READ;
        if (vm_flags & VM_WRITE) perm |= PTE_W;
        if ((ret = mm_map(mm, ph->p_va, ph->p_memsz, vm_flags, NULL)) != 0) {
            goto bad_cleanup_mmap;
        }
        off_t offset = ph->p_offset;
        size_t off, size;
        uintptr_t start = ph->p_va, end, la = ROUNDDOWN(start, PGSIZE);

        ret = -E_NO_MEM;

        end = ph->p_va + ph->p_filesz;
        while (start < end) {
            if ((page = pgdir_alloc_page(mm->pgdir, la, perm)) == NULL) {
                ret = -E_NO_MEM;
                goto bad_cleanup_mmap;
            }
            off = start - la, size = PGSIZE - off, la += PGSIZE;
            if (end < la) {
                size -= la - end;
            }
            if ((ret = load_icode_read(fd, page2kva(page) + off, size, offset)) != 0) {
                goto bad_cleanup_mmap;
            }
            start += size, offset += size;
        }
        end = ph->p_va + ph->p_memsz;

        if (start < la) {
            /* ph->p_memsz == ph->p_filesz */
            if (start == end) {
                continue ;
            }
            off = start + PGSIZE - la, size = PGSIZE - off;
            if (end < la) {
                size -= la - end;
            }
            memset(page2kva(page) + off, 0, size);
            start += size;
            assert((end < la && start == end) || (end >= la && start == la));
        }
        while (start < end) {
            if ((page = pgdir_alloc_page(mm->pgdir, la, perm)) == NULL) {
                ret = -E_NO_MEM;
                goto bad_cleanup_mmap;
            }
            off = start - la, size = PGSIZE - off, la += PGSIZE;
            if (end < la) {
                size -= la - end;
            }
            memset(page2kva(page) + off, 0, size);
            start += size;
        }
    }
    sysfile_close(fd);

    vm_flags = VM_READ | VM_WRITE | VM_STACK;
    if ((ret = mm_map(mm, USTACKTOP - USTACKSIZE, USTACKSIZE, vm_flags, NULL)) != 0) {
        goto bad_cleanup_mmap;
    }
    assert(pgdir_alloc_page(mm->pgdir, USTACKTOP-PGSIZE , PTE_USER) != NULL);
    assert(pgdir_alloc_page(mm->pgdir, USTACKTOP-2*PGSIZE , PTE_USER) != NULL);
    assert(pgdir_alloc_page(mm->pgdir, USTACKTOP-3*PGSIZE , PTE_USER) != NULL);
    assert(pgdir_alloc_page(mm->pgdir, USTACKTOP-4*PGSIZE , PTE_USER) != NULL);
    
    mm_count_inc(mm);
    current->mm = mm;
    current->cr3 = PADDR(mm->pgdir);
    lcr3(PADDR(mm->pgdir));

    //setup argc, argv
    uint32_t argv_size=0, i;
    for (i = 0; i < argc; i ++) {
        argv_size += strnlen(kargv[i],EXEC_MAX_ARG_LEN + 1)+1;
    }

    uintptr_t stacktop = USTACKTOP - (argv_size/sizeof(long)+1)*sizeof(long);
    char** uargv=(char **)(stacktop  - argc * sizeof(char *));
    
    argv_size = 0;
    for (i = 0; i < argc; i ++) {
        uargv[i] = strcpy((char *)(stacktop + argv_size ), kargv[i]);
        argv_size +=  strnlen(kargv[i],EXEC_MAX_ARG_LEN + 1)+1;
    }
    
    stacktop = (uintptr_t)uargv - sizeof(int);
    *(int *)stacktop = argc;
    
    struct trapframe *tf = current->tf;
    memset(tf, 0, sizeof(struct trapframe));
    tf->tf_cs = USER_CS;
    tf->tf_ds = tf->tf_es = tf->tf_ss = USER_DS;
    tf->tf_esp = stacktop;
    tf->tf_eip = elf->e_entry;
    tf->tf_eflags = FL_IF;
    ret = 0;
out:
    return ret;
bad_cleanup_mmap:
    exit_mmap(mm);
bad_elf_cleanup_pgdir:
    put_pgdir(mm);
bad_pgdir_cleanup_mm:
    mm_destroy(mm);
bad_mm:
    goto out;
}

// this function isn't very correct in LAB8
static void
put_kargv(int argc, char **kargv) {
    while (argc > 0) {
        kfree(kargv[-- argc]);
    }
}

static int
copy_kargv(struct mm_struct *mm, int argc, char **kargv, const char **argv) {
    int i, ret = -E_INVAL;
    if (!user_mem_check(mm, (uintptr_t)argv, sizeof(const char *) * argc, 0)) {
        return ret;
    }
    for (i = 0; i < argc; i ++) {
        char *buffer;
        if ((buffer = kmalloc(EXEC_MAX_ARG_LEN + 1)) == NULL) {
            goto failed_nomem;
        }
        if (!copy_string(mm, buffer, argv[i], EXEC_MAX_ARG_LEN + 1)) {
            kfree(buffer);
            goto failed_cleanup;
        }
        kargv[i] = buffer;
    }
    return 0;

failed_nomem:
    ret = -E_NO_MEM;
failed_cleanup:
    put_kargv(i, kargv);
    return ret;
}

// do_execve - call exit_mmap(mm)&put_pgdir(mm) to reclaim memory space of current process
//           - call load_icode to setup new memory space accroding binary prog.
int
do_execve(const char *name, int argc, const char **argv) {
    static_assert(EXEC_MAX_ARG_LEN >= FS_MAX_FPATH_LEN);
    struct mm_struct *mm = current->mm;
    if (!(argc >= 1 && argc <= EXEC_MAX_ARG_NUM)) {
        return -E_INVAL;
    }

    char local_name[PROC_NAME_LEN + 1];
    memset(local_name, 0, sizeof(local_name));
    
    char *kargv[EXEC_MAX_ARG_NUM];
    const char *path;
    
    int ret = -E_INVAL;
    
    lock_mm(mm);
    if (name == NULL) {
        snprintf(local_name, sizeof(local_name), "<null> %d", current->pid);
    }
    else {
        if (!copy_string(mm, local_name, name, sizeof(local_name))) {
            unlock_mm(mm);
            return ret;
        }
    }
    if ((ret = copy_kargv(mm, argc, kargv, argv)) != 0) {
        unlock_mm(mm);
        return ret;
    }
    path = argv[0];
    unlock_mm(mm);
    files_closeall(current->filesp);

    /* sysfile_open will check the first argument path, thus we have to use a user-space pointer, and argv[0] may be incorrect */    
    int fd;
    if ((ret = fd = sysfile_open(path, O_RDONLY)) < 0) {
        goto execve_exit;
    }
    if (mm != NULL) {
        lcr3(boot_cr3);
        if (mm_count_dec(mm) == 0) {
            exit_mmap(mm);
            put_pgdir(mm);
            mm_destroy(mm);
        }
        current->mm = NULL;
    }
    ret= -E_NO_MEM;;
    if ((ret = load_icode(fd, argc, kargv)) != 0) {
        goto execve_exit;
    }
    put_kargv(argc, kargv);
    set_proc_name(current, local_name);
    return 0;

execve_exit:
    put_kargv(argc, kargv);
    do_exit(ret);
    panic("already exit: %e.\n", ret);
}

// do_yield - ask the scheduler to reschedule
int
do_yield(void) {
    current->need_resched = 1;     //表示可以释放cpu资源
    return 0;
}

// do_wait - wait one OR any children with PROC_ZOMBIE state, and free memory space of kernel stack
//         - proc struct of this child.
// NOTE: only after do_wait function, all resources of the child proces are free.
int
do_wait(int pid, int *code_store) {
    struct mm_struct *mm = current->mm;
    if (code_store != NULL) {
        if (!user_mem_check(mm, (uintptr_t)code_store, sizeof(int), 1)) {
            return -E_INVAL;
        }
    }

    struct proc_struct *proc;
    bool intr_flag, haskid;
repeat:
    haskid = 0;
    if (pid != 0) {
        proc = find_proc(pid);
        if (proc != NULL && proc->parent == current) {
            haskid = 1;
            if (proc->state == PROC_ZOMBIE) {
                goto found;
            }
        }
    }
    else {
        proc = current->cptr;
        for (; proc != NULL; proc = proc->optr) {
            haskid = 1;
            if (proc->state == PROC_ZOMBIE) {
                goto found;
            }
        }
    }
    if (haskid) {
        current->state = PROC_SLEEPING;   //将进程状态变为睡眠
        current->wait_state = WT_CHILD;
        schedule();
        if (current->flags & PF_EXITING) {
            do_exit(-E_KILLED);
        }
        goto repeat;
    }
    return -E_BAD_PROC;

found:
    if (proc == idleproc || proc == initproc) {
        panic("wait idleproc or initproc.\n");
    }
    if (code_store != NULL) {
        *code_store = proc->exit_code;
    }
    local_intr_save(intr_flag);
    {
        unhash_proc(proc);
        remove_links(proc);
    }
    local_intr_restore(intr_flag);
    put_kstack(proc);
    kfree(proc);
    return 0;
}

// do_kill - kill process with pid by set this process's flags with PF_EXITING
//杀死进程
int
do_kill(int pid) {
    list_entry_t *lec = &proc_list;
    list_entry_t *pbc = &proc_list;
    struct proc_struct* pbcw;
    while (list_next(lec) != pbc){
       pbcw = le2proc(list_next(lec), list_link);
       if(pbcw->pid==pid)
       {lec->next=lec->next->next;}
       break;                      //将进程从进程队列中删除
    }

    struct proc_struct *proc;
    if ((proc = find_proc(pid)) != NULL) {
        if (!(proc->flags & PF_EXITING)) {
            proc->flags |= PF_EXITING;
            if (proc->wait_state & WT_INTERRUPTED) {
                wakeup_proc(proc);
            }
            return 0;
        }
        return -E_KILLED;
    }
    return -E_INVAL;
}

// kernel_execve - do SYS_exec syscall to exec a user program called by user_main kernel_thread
static int
kernel_execve(const char *name, const char **argv) {
    int argc = 0, ret;
    while (argv[argc] != NULL) {
        argc ++;
    }
    asm volatile (
        "int %1;"
        : "=a" (ret)
        : "i" (T_SYSCALL), "0" (SYS_exec), "d" (name), "c" (argc), "b" (argv)
        : "memory");
    return ret;
}

#define __KERNEL_EXECVE(name, path, ...) ({                         \
const char *argv[] = {path, ##__VA_ARGS__, NULL};       \
                     cprintf("kernel_execve: pid = %d, name = \"%s\".\n",    \
                             current->pid, name);                            \
                     kernel_execve(name, argv);                              \
})

#define KERNEL_EXECVE(x, ...)                   __KERNEL_EXECVE(#x, #x, ##__VA_ARGS__)

#define KERNEL_EXECVE2(x, ...)                  KERNEL_EXECVE(x, ##__VA_ARGS__)

#define __KERNEL_EXECVE3(x, s, ...)             KERNEL_EXECVE(x, #s, ##__VA_ARGS__)

#define KERNEL_EXECVE3(x, s, ...)               __KERNEL_EXECVE3(x, s, ##__VA_ARGS__)

// user_main - kernel thread used to exec a user program
static int
user_main(void *arg) {
#ifdef TEST
#ifdef TESTSCRIPT
    KERNEL_EXECVE3(TEST, TESTSCRIPT);
#else
    KERNEL_EXECVE2(TEST);
#endif
#else
    KERNEL_EXECVE(sh);
#endif
    panic("user_main execve failed.\n");
}

// init_main - the second kernel thread used to create user_main kernel threads
static int
init_main(void *arg) {
    int ret;
    if ((ret = vfs_set_bootfs("disk0:")) != 0) {
        panic("set boot fs failed: %e.\n", ret);
    }
    
    size_t nr_free_pages_store = nr_free_pages();
    size_t kernel_allocated_store = kallocated();

    int pid = kernel_thread(user_main, NULL, 0);
    if (pid <= 0) {
        panic("create user_main failed.\n");
    }
// extern void check_sync(void);
//    check_sync();                // check philosopher sync problem

    while (do_wait(0, NULL) == 0) {
        schedule();
    }

    fs_cleanup();
        
    cprintf("all user-mode processes have quit.\n");
    assert(initproc->cptr == NULL && initproc->yptr == NULL && initproc->optr == NULL);
    assert(nr_process == 2);
    assert(list_next(&proc_list) == &(initproc->list_link));
    assert(list_prev(&proc_list) == &(initproc->list_link));
    assert(nr_free_pages_store == nr_free_pages());
    assert(kernel_allocated_store == kallocated());
    cprintf("init check memory pass.\n");
    return 0;
}

// proc_init - set up the first kernel thread idleproc "idle" by itself and 
//           - create the second kernel thread init_main
void      //创建init_main线程
proc_init(void) {
    int i;

    list_init(&proc_list);
    for (i = 0; i < HASH_LIST_SIZE; i ++) {
        list_init(hash_list + i);
    }

    if ((idleproc = alloc_proc()) == NULL) {
        panic("cannot alloc idleproc.\n");
    }

    idleproc->pid = 0;            //将状态初始化
    idleproc->state = PROC_RUNNABLE;
    idleproc->kstack = (uintptr_t)bootstack;
    idleproc->need_resched = 1;
    
    if ((idleproc->filesp = files_create()) == NULL) {
        panic("create filesp (idleproc) failed.\n");
    }
    files_count_inc(idleproc->filesp);
    
    set_proc_name(idleproc, "idle");
    nr_process ++;
    char* std[4]={"PROC_UNINIT","PROC_SLEEPING","PROC_RUNNABLE","PROC_ZOMBIE"};
    current = idleproc;
    cprintf("create idleproc ... \n");                                           //打印出创建出的内核进程的状态
    cprintf("pid:%d    state:%s    name:%s    time_slice:%d    runtime:%d\n",  //分别打印出进程pid,状态，名称，时间片，以及运行时间
        current->pid,std[current->state],current->name,current->time_slice,current->runs);

    int pid = kernel_thread(init_main, NULL, 0);
    if (pid <= 0) {
        panic("create init_main failed.\n");
    }

    initproc = find_proc(pid);
    set_proc_name(initproc, "init");

    cprintf("create initproc ... \n");
    cprintf("pid:%d    state:%s    name:%s    time_slice:%d    runtime:%d\n",
        initproc->pid,std[initproc->state],initproc->name,initproc->time_slice,initproc->runs);

    assert(idleproc != NULL && idleproc->pid == 0);
    assert(initproc != NULL && initproc->pid == 1);
}
// cpu_idle - at the end of kern_init, the first kernel thread idleproc will do below works
void
cpu_idle(void) {
    while (1) {
        if (current->need_resched) {
            schedule();
        }
    }
}


void
lab6_set_priority(uint32_t priority)
{
    if (priority == 0)
        current->priority = 1;
    else current->priority = priority;
}

// do_sleep - set current process state to sleep and add timer with "time"
//          - then call scheduler. if process run again, delete timer first.
int
do_sleep(unsigned int time) {
    if (time == 0) {
        return 0;
    }
    bool intr_flag;
    local_intr_save(intr_flag);
    timer_t __timer, *timer = timer_init(&__timer, current, time);
    current->state = PROC_SLEEPING;
    current->wait_state = WT_TIMER;
    add_timer(timer);
    local_intr_restore(intr_flag);

    schedule();

    del_timer(timer);
    return 0;
}

int
do_malloc(int n){
    struct mm_struct *mm = current->mm;
    assert(mm != NULL);

    //(4K 1页的话，如果要求4K,相当于多分配一页，页数就是alloc_size
    int alloc_size = n/(PGSIZE);
    if(!n%(PGSIZE)){
    	alloc_size += 1;
    }

    //就是再往mmap_list里面添加几个vma_struct 就行, 如果最小分配单位是Page
    list_entry_t *le = &(mm->mmap_list);
    print_current_vm();



    le = list_prev(le);
    struct vma_struct *next_vma = le2vma(le, list_link);
    le = list_prev(le);
    struct vma_struct *last_vma = le2vma(le, list_link);

    uintptr_t start = last_vma->vm_end + 0x1;
    uintptr_t end = start + alloc_size * PGSIZE;

    uint32_t new_flags = last_vma->vm_flags;

    start = ROUNDDOWN(start, PGSIZE), end = ROUNDUP(end, PGSIZE);

    if(start > last_vma->vm_end && end < next_vma->vm_start){
	return -E_NO_MEM;
    }

    if (!USER_ACCESS(start, end)) {
        return -E_INVAL;
    }


    struct vma_struct *new_vma = vma_create(start, end, new_flags);
    if (new_vma == NULL) {
        return -E_NO_MEM;
    }

    insert_vma_struct(mm, new_vma);
    print_current_vm();
    return 0;
}
//此函数用于列出当前存在的所有进程的PCB中的一些信息
void
print_listProc(void){
    char* std[4]={"PROC_UNINIT","PROC_SLEEPING","PROC_RUNNABLE","PROC_ZOMBIE"};   //进程的四个状态
    cprintf("\n");
    list_entry_t *lec = &proc_list;
    list_entry_t *pbc = &proc_list;
    struct proc_struct* pbcw;
    while ((lec = list_next(lec)) != pbc){     //遍历进程队列
        pbcw = le2proc(lec, list_link);
        list_entry_t *lem=&pbcw->mm->mmap_list;
        list_entry_t *pbm=&pbcw->mm->mmap_list;
        struct vma_struct* pbmw;
        cprintf("pid:%d    state:%s    name:%s    time_slice:%d    runtime:%d\n",pbcw->pid,std[pbcw->state],pbcw->name,pbcw->time_slice,pbcw->runs);
        if(pbcw->pid!=1&&pbcw->state!=3){
            while((lem = list_next(lem)) != pbm){
		    pbmw=le2vma(lem,list_link);
		    cprintf("\t size:%x [ 0x%x - 0x%x ] \n",(pbmw->vm_end - pbmw->vm_start)/4096, pbmw->vm_start,pbmw->vm_end);
            }
        }            
        cprintf("\n");
    }

}


void
print_current_vm(void){
    char* std[4]={"PROC_UNINIT","PROC_SLEEPING","PROC_RUNNABLE","PROC_ZOMBIE"};
    cprintf("\n");
	
    list_entry_t *le = &current->mm->mmap_list;
    list_entry_t *head = &current->mm->mmap_list;

    struct vma_struct *vms;
    cprintf("%d :%s :\n",current->pid, std[current->state]);
    
    while((le = list_next(le)) != head){
	vms = le2vma(le, list_link);
	cprintf("\t size:%x [ 0x%x - 0x%x ] \n",(vms->vm_end - vms->vm_start)/4096, vms->vm_start, vms->vm_end);
    }

    cprintf("\n");
}


void do_mem(void){
    cprintf("the amount of physical memory (in pages) : %d \n", npage);

    struct mm_struct *mm = current->mm;
    list_entry_t *head = &mm->mmap_list;

	uint32_t num_of_vp = 0;
	uint32_t num_of_pp = 0;
    list_entry_t *le = &mm->mmap_list;
    //看一下虚拟地址对应的物理地址
    while((le = list_next(le)) != &(mm->mmap_list)){
        struct vma_struct *temp_vma = le2vma(le, list_link);
        uintptr_t start_addr = temp_vma->vm_start;
        uintptr_t end_addr = start_addr + PGSIZE;
	uintptr_t end_up = ROUNDUP(temp_vma->vm_end, PGSIZE);
	
	for(;end_addr <= end_up ; start_addr+=PGSIZE, end_addr+=PGSIZE, num_of_vp+=1){
	        pte_t *pes = get_pte(mm->pgdir ,start_addr, 0);
        
		//PDX(start_addr) 该地址在页目录表中的索引， &pgdir[PDX(start_addr)] 页表地址

		uintptr_t phaddr = (*pes & ~0xFFF)  + (start_addr & 0xFFF);

		//前20位对应物理块号，怎么找物理地址

		assert(pes != NULL);
		if(phaddr != 0)
			num_of_pp++;
		cprintf("virtual address [ 0x%x - 0x%x ] ----> physical address [ 0x%x ] \n", start_addr, end_addr, phaddr);
	}
	
    }
    cprintf("num of virtual page for this process (in pages) : %d \n", num_of_vp);
    cprintf("num of physical page for this prcess (in pages) : %d \n", num_of_pp);
}

void do_list_wait(void){
    list_entry_t *lec = &proc_list;
    list_entry_t *pbc = &proc_list;
    struct proc_struct* pbcw;
    while ((lec = list_next(lec)) != pbc){
        cprintf("\n");
        pbcw = le2proc(lec, list_link);
        if(pbcw->wait_state==WT_KBD)
        {
            cprintf("%d process is waiting IO!",pbcw->pid);
        }
        else if(pbcw->wait_state==WT_TIMER)
        {
            cprintf("%d process is waiting TIMER!",pbcw->pid);
        }
        else if(pbcw->wait_state==WT_CHILD)
        {
            cprintf("%d process is waiting CHILD!",pbcw->pid);
        }
        else if(pbcw->pid != current->pid)
        {
            cprintf("%d process is waiting CPU!",pbcw->pid);
        }
    }
    cprintf("\n-----------------\n");
}

