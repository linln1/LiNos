#include <defs.h>
#include <x86.h>
#include <stab.h>
#include <stdio.h>
#include <string.h>
#include <memlayout.h>
#include <sync.h>
#include <vmm.h>
#include <proc.h>
#include <kdebug.h>
#include <kmonitor.h>
#include <assert.h>

#define STACKFRAME_DEPTH 20

extern const struct stab __STAB_BEGIN__[];  // beginning of stabs table
extern const struct stab __STAB_END__[];    // end of stabs table
extern const char __STABSTR_BEGIN__[];      // beginning of string table
extern const char __STABSTR_END__[];        // end of string table

/* debug information about a particular instruction pointer */
struct eipdebuginfo {
    const char *eip_file;                   // eip的源代码文件名 
    int eip_line;                           // eip的源代码行号
    const char *eip_fn_name;               // 包含eip的函数名   
    int eip_fn_namelen;                     // 函数名的长度 
    uintptr_t eip_fn_addr;                  // 函数的起始地址  
    int eip_fn_narg;                        // 函数参数的数量
};

/* user STABS data structure  */
struct userstabdata {
    const struct stab *stabs;
    const struct stab *stab_end;
    const char *stabstr;
    const char *stabstr_end;
};

//stab_binsearch用于查找一个包含地址addr而且匹配类型type的一个单一stab条目，将符合addr的最小范围括起来。
static void
stab_binsearch(const struct stab *stabs, int *region_left, int *region_right,
           int type, uintptr_t addr) {
    int l = *region_left, r = *region_right, any_matches = 0;

    while (l <= r) {
        int true_m = (l + r) / 2, m = true_m;

        // 用正确的类型搜索最早的stab
        while (m >= l && stabs[m].n_type != type) {
            m --;
        }
        if (m < l) {    // no match in [l, m]
            l = true_m + 1;
            continue;
        }

        // 二分查找
        any_matches = 1;
        if (stabs[m].n_value < addr) {
            *region_left = m;
            l = true_m + 1;
        } else if (stabs[m].n_value > addr) {
            *region_right = m - 1;
            r = m - 1;
        } else {
             // 精确匹配'addr'，但继续循环查找* region_right
            *region_left = m;
            l = m;
            addr ++;
        }
    }

    if (!any_matches) {
        *region_right = *region_left - 1;
    }
    else {
            // 查找包含“addr”的最右边区域
        l = *region_right;
        for (; l > *region_left && stabs[l].n_type != type; l --)
            /* do nothing */;
        *region_left = l;
    }
}

//debuginfo_eip用于在eipdebuginfo类型的结构info中填入有关的指令地址addr。若信息找到则返回0，否则返回负数。但info中总会存入一些信息。
int
debuginfo_eip(uintptr_t addr, struct eipdebuginfo *info) {
    const struct stab *stabs, *stab_end;
    const char *stabstr, *stabstr_end;

    info->eip_file = "<unknown>";
    info->eip_line = 0;
    info->eip_fn_name = "<unknown>";
    info->eip_fn_namelen = 9;
    info->eip_fn_addr = addr;
    info->eip_fn_narg = 0;

    // find the relevant set of stabs
    if (addr >= KERNBASE) {
        stabs = __STAB_BEGIN__;
        stab_end = __STAB_END__;
        stabstr = __STABSTR_BEGIN__;
        stabstr_end = __STABSTR_END__;
    }
    else {
        // user-program linker script, tools/user.ld puts the information about the
        // program's stabs (included __STAB_BEGIN__, __STAB_END__, __STABSTR_BEGIN__,
        // and __STABSTR_END__) in a structure located at virtual address USTAB.
        const struct userstabdata *usd = (struct userstabdata *)USTAB;

        // make sure that debugger (current process) can access this memory
        struct mm_struct *mm;
        if (current == NULL || (mm = current->mm) == NULL) {
            return -1;
        }
        if (!user_mem_check(mm, (uintptr_t)usd, sizeof(struct userstabdata), 0)) {
            return -1;
        }

        stabs = usd->stabs;
        stab_end = usd->stab_end;
        stabstr = usd->stabstr;
        stabstr_end = usd->stabstr_end;

        // make sure the STABS and string table memory is valid
        if (!user_mem_check(mm, (uintptr_t)stabs, (uintptr_t)stab_end - (uintptr_t)stabs, 0)) {
            return -1;
        }
        if (!user_mem_check(mm, (uintptr_t)stabstr, stabstr_end - stabstr, 0)) {
            return -1;
        }
    }

    // String table validity checks
    if (stabstr_end <= stabstr || stabstr_end[-1] != 0) {
        return -1;
    }

    // Now we find the right stabs that define the function containing
    // 'eip'.  First, we find the basic source file containing 'eip'.
    // Then, we look in that source file for the function.  Then we look
    // for the line number.

    // Search the entire set of stabs for the source file (type N_SO).
    int lfile = 0, rfile = (stab_end - stabs) - 1;
    stab_binsearch(stabs, &lfile, &rfile, N_SO, addr);
    if (lfile == 0)
        return -1;

    // Search within that file's stabs for the function definition
    // (N_FUN).
    int lfun = lfile, rfun = rfile;
    int lline, rline;
    stab_binsearch(stabs, &lfun, &rfun, N_FUN, addr);

    if (lfun <= rfun) {
        // stabs[lfun] points to the function name
        // in the string table, but check bounds just in case.
        if (stabs[lfun].n_strx < stabstr_end - stabstr) {
            info->eip_fn_name = stabstr + stabs[lfun].n_strx;
        }
        info->eip_fn_addr = stabs[lfun].n_value;
        addr -= info->eip_fn_addr;
        // Search within the function definition for the line number.
        lline = lfun;
        rline = rfun;
    } else {
        // Couldn't find function stab!  Maybe we're in an assembly
        // file.  Search the whole file for the line number.
        info->eip_fn_addr = addr;
        lline = lfile;
        rline = rfile;
    }
    info->eip_fn_namelen = strfind(info->eip_fn_name, ':') - info->eip_fn_name;

    // Search within [lline, rline] for the line number stab.
    // If found, set info->eip_line to the right line number.
    // If not found, return -1.
    stab_binsearch(stabs, &lline, &rline, N_SLINE, addr);
    if (lline <= rline) {
        info->eip_line = stabs[rline].n_desc;
    } else {
        return -1;
    }

    // Search backwards from the line number for the relevant filename stab.
    // We can't just use the "lfile" stab because inlined functions
    // can interpolate code from a different file!
    // Such included source files use the N_SOL stab type.
    while (lline >= lfile
           && stabs[lline].n_type != N_SOL
           && (stabs[lline].n_type != N_SO || !stabs[lline].n_value)) {
        lline --;
    }
    if (lline >= lfile && stabs[lline].n_strx < stabstr_end - stabstr) {
        info->eip_file = stabstr + stabs[lline].n_strx;
    }

    // Set eip_fn_narg to the number of arguments taken by the function,
    // or 0 if there was no containing function.
    if (lfun < rfun) {
        for (lline = lfun + 1;
             lline < rfun && stabs[lline].n_type == N_PSYM;
             lline ++) {
            info->eip_fn_narg ++;
        }
    }
    return 0;
}

//print_kerninfo负责打印关于内核的信息，包括内核条目的位置、数据段和文本段的起始地址、空闲内存的起始地址以及内核已经使用了多少内存。
void
print_kerninfo(void) {
    extern char etext[], edata[], end[], kern_init[];
    cprintf("1. 打印内核镜像信息 \n");
    cprintf("Special kernel symbols:\n");
    cprintf("  entry  0x%08x (phys)\n", kern_init);
    cprintf("  etext  0x%08x (phys)\n", etext);
    cprintf("  edata  0x%08x (phys)\n", edata);
    cprintf("  end    0x%08x (phys)\n", end);
    cprintf("Kernel executable memory footprint: %dKB\n", (end - kern_init + 1023)/1024);
}


void
print_osinfo(void){

	cprintf("         _______                   _____          \n");
	cprintf("        /::\\    \\                 /\\    \\         \n");
	cprintf("       /::::\\    \\               /::\\    \\        \n");
	cprintf("      /::::::\\    \\             /::::\\    \\       \n");
	cprintf("     /::::::::\\    \\           /::::::\\    \\      \n");
	cprintf("    /:::/~~\\:::\\    \\         /:::/\\:::\\    \\     \n");
	cprintf("   /:::/    \\:::\\    \\       /:::/__\\:::\\    \\    \n");
	cprintf("  /:::/    / \\:::\\    \\      \\:::\\   \\:::\\    \\   \n");
	cprintf(" /:::/____/   \\:::\\____\\   ___\\:::\\   \\:::\\    \\  \n");
	cprintf("|:::|    |     |:::|    | /\\   \\:::\\   \\:::\\    \\ \n");
	cprintf("|:::|____|     |:::|    |/::\\   \\:::\\   \\:::\\____\\ \n");
	cprintf(" \\:::\\    \\   /:::/    / \\:::\\   \\:::\\   \\::/    /\n");
	cprintf("  \\:::\\    \\ /:::/    /   \\:::\\   \\:::\\   \\/____/ \n");
	cprintf("   \\:::\\    /:::/    /     \\:::\\   \\:::\\    \\     \n");
	cprintf("    \\:::\\__/:::/    /       \\:::\\   \\:::\\____\\    \n");
	cprintf("     \\::::::::/    /         \\:::\\  /:::/    /    \n");
	cprintf("      \\::::::/    /           \\:::\\/:::/    /     \n");
	cprintf("       \\::::/    /             \\::::::/    /      \n");
	cprintf("        \\::/____/               \\::::/    /       \n");
	cprintf("         ~~                      \\::/    /        \n");
	cprintf("                                  \\/____/     \n");
	cprintf("           @Copyright is based on ucore            \n");

	cprintf(" * Virtual memory map:                                          Permissions\n");
	cprintf(" *                                                              kernel/user\n");
	cprintf(" *\n");
	cprintf(" *     4G ------------------> +---------------------------------+\n");
	cprintf(" *                            |                                 |\n");
	cprintf(" *                            |         Empty Memory (*)        |\n");
	cprintf(" *                            |                                 |\n");
	cprintf(" *                            +---------------------------------+ 0xFB000000\n");
	cprintf(" *                            |   Cur. Page Table (Kern, RW)    | RW/-- PTSIZE\n");
	cprintf(" *     VPT -----------------> +---------------------------------+ 0xFAC00000\n");
	cprintf(" *                            |        Invalid Memory (*)       | --/--\n");
	cprintf(" *     KERNTOP -------------> +---------------------------------+ 0xF8000000\n");
	cprintf(" *                            |                                 |\n");
	cprintf(" *                            |    Remapped Physical Memory     | RW/-- KMEMSIZE\n");
	cprintf(" *                            |                                 |\n");
	cprintf(" *     KERNBASE ------------> +---------------------------------+ 0xC0000000\n");
	cprintf(" *                            |        Invalid Memory (*)       | --/--\n");
	cprintf(" *     USERTOP -------------> +---------------------------------+ 0xB0000000\n");
	cprintf(" *                            |           User stack            |\n");
	cprintf(" *                            +---------------------------------+\n");
	cprintf(" *                            |                                 |\n");
	cprintf(" *                            :                                 :\n");
	cprintf(" *                            |         ~~~~~~~~~~~~~~~~        |\n");
	cprintf(" *                            :                                 :\n");
	cprintf(" *                            |                                 |\n");
	cprintf(" *                            ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	cprintf(" *                            |       User Program & Heap       |\n");
	cprintf(" *     UTEXT ---------------> +---------------------------------+ 0x00800000\n");
	cprintf(" *                            |        Invalid Memory (*)       | --/--\n");
	cprintf(" *                            |  - - - - - - - - - - - - - - -  |\n");
	cprintf(" *                            |    User STAB Data (optional)    |\n");
	cprintf(" *     USERBASE, USTAB------> +---------------------------------+ 0x00200000\n");
	cprintf(" *                            |        Invalid Memory (*)       | --/--\n");
	cprintf(" *     0 -------------------> +---------------------------------+ 0x00000000\n");

	cprintf("Co-Author : GCW GPF HHY LZY WYF YZJ \n");

}

/* *
 * print_debuginfo - read and print the stat information for the address @eip,
 * and info.eip_fn_addr should be the first address of the related function.
 * */
void
print_debuginfo(uintptr_t eip) {
    struct eipdebuginfo info;
    if (debuginfo_eip(eip, &info) != 0) {
        cprintf("    <unknow>: -- 0x%08x --\n", eip);
    }
    else {
        char fnname[256];
        int j;
        for (j = 0; j < info.eip_fn_namelen; j ++) {
            fnname[j] = info.eip_fn_name[j];
        }
        fnname[j] = '\0';
        cprintf("    %s:%d: %s+%d\n", info.eip_file, info.eip_line,
                fnname, eip - info.eip_fn_addr);
    }
}

static __noinline uint32_t
read_eip(void) {
    uint32_t eip;
    asm volatile("movl 4(%%ebp), %0" : "=r" (eip));
    return eip;
}


void
print_stackframe(void) {
    uint32_t ebp = read_ebp(), eip = read_eip();

    int i, j;
    for (i = 0; ebp != 0 && i < STACKFRAME_DEPTH; i ++) {
        cprintf("ebp:0x%08x eip:0x%08x args:", ebp, eip);
        uint32_t *args = (uint32_t *)ebp + 2;
        for (j = 0; j < 4; j ++) {
            cprintf("0x%08x ", args[j]);
        }
        cprintf("\n");
        print_debuginfo(eip - 1);
        eip = ((uint32_t *)ebp)[1];
        ebp = ((uint32_t *)ebp)[0];
    }
}

