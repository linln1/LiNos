#ifndef _ELF_H
#define _ELF_H

#include <../libs/ctype.h>
#define ELF_MAGIC_NUMBER 0x464C457FU

/* ELF header */
struct elf_header {
	uint32_t elf_magic_number;	
	uint8_t elf[12];
    uint16_t elf_type;      // 1=可重定位, 2=可执行, 3=共享目标文件, 4=内核镜像文件
    uint16_t elf_machine;   // 3=x86
    uint32_t elf_version;   // 版本 1
    uint32_t elf_entry;     // 
    uint32_t elf_phoff;     // file position of program header or 0
    uint32_t elf_shoff;     // file position of section header or 0
    uint32_t elf_flags;     // 
    uint16_t elf_ehsize;    // elf header大小
    uint16_t elf_phentsize; // 
    uint16_t elf_phnum;     // 
    uint16_t elf_shentsize; // 
    uint16_t elf_shnum;     // 
    uint16_t elf_shstrndx;  // 
};

/* program section header */
struct proghdr {
    uint32_t prog_type;   // 
    uint32_t prog_offset; // 
    uint32_t prog_va;     // 
    uint32_t prog_pa;     // 
    uint32_t prog_filesz; // 
    uint32_t prog_memsz;  // 
    uint32_t prog_flags;  // read/write/execute bits
    uint32_t prog_align;  // required alignment, invariably hardware page size
};

#endif /* !__LIBS_ELF_H__ */