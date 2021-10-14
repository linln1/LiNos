
obj/__user_badsegment.out：     文件格式 elf32-i386


Disassembly of section .text:

00800020 <opendir>:
#include <error.h>
#include <unistd.h>

DIR dir, *dirp=&dir;
DIR *
opendir(const char *path) {
  800020:	55                   	push   %ebp
  800021:	89 e5                	mov    %esp,%ebp
  800023:	53                   	push   %ebx
  800024:	83 ec 34             	sub    $0x34,%esp

    if ((dirp->fd = open(path, O_RDONLY)) < 0) {
  800027:	8b 1d 00 20 80 00    	mov    0x802000,%ebx
  80002d:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
  800034:	00 
  800035:	8b 45 08             	mov    0x8(%ebp),%eax
  800038:	89 04 24             	mov    %eax,(%esp)
  80003b:	e8 de 00 00 00       	call   80011e <open>
  800040:	89 03                	mov    %eax,(%ebx)
  800042:	8b 03                	mov    (%ebx),%eax
  800044:	85 c0                	test   %eax,%eax
  800046:	79 02                	jns    80004a <opendir+0x2a>
        goto failed;
  800048:	eb 44                	jmp    80008e <opendir+0x6e>
    }
    struct stat __stat, *stat = &__stat;
  80004a:	8d 45 e4             	lea    -0x1c(%ebp),%eax
  80004d:	89 45 f4             	mov    %eax,-0xc(%ebp)
    if (fstat(dirp->fd, stat) != 0 || !S_ISDIR(stat->st_mode)) {
  800050:	a1 00 20 80 00       	mov    0x802000,%eax
  800055:	8b 00                	mov    (%eax),%eax
  800057:	8b 55 f4             	mov    -0xc(%ebp),%edx
  80005a:	89 54 24 04          	mov    %edx,0x4(%esp)
  80005e:	89 04 24             	mov    %eax,(%esp)
  800061:	e8 62 01 00 00       	call   8001c8 <fstat>
  800066:	85 c0                	test   %eax,%eax
  800068:	75 24                	jne    80008e <opendir+0x6e>
  80006a:	8b 45 f4             	mov    -0xc(%ebp),%eax
  80006d:	8b 00                	mov    (%eax),%eax
  80006f:	25 00 70 00 00       	and    $0x7000,%eax
  800074:	3d 00 20 00 00       	cmp    $0x2000,%eax
  800079:	75 13                	jne    80008e <opendir+0x6e>
        goto failed;
    }
    dirp->dirent.offset = 0;
  80007b:	a1 00 20 80 00       	mov    0x802000,%eax
  800080:	c7 40 04 00 00 00 00 	movl   $0x0,0x4(%eax)
    return dirp;
  800087:	a1 00 20 80 00       	mov    0x802000,%eax
  80008c:	eb 05                	jmp    800093 <opendir+0x73>

failed:
    return NULL;
  80008e:	b8 00 00 00 00       	mov    $0x0,%eax
}
  800093:	83 c4 34             	add    $0x34,%esp
  800096:	5b                   	pop    %ebx
  800097:	5d                   	pop    %ebp
  800098:	c3                   	ret    

00800099 <readdir>:

struct dirent *
readdir(DIR *dirp) {
  800099:	55                   	push   %ebp
  80009a:	89 e5                	mov    %esp,%ebp
  80009c:	83 ec 18             	sub    $0x18,%esp
    if (sys_getdirentry(dirp->fd, &(dirp->dirent)) == 0) {
  80009f:	8b 45 08             	mov    0x8(%ebp),%eax
  8000a2:	8d 50 04             	lea    0x4(%eax),%edx
  8000a5:	8b 45 08             	mov    0x8(%ebp),%eax
  8000a8:	8b 00                	mov    (%eax),%eax
  8000aa:	89 54 24 04          	mov    %edx,0x4(%esp)
  8000ae:	89 04 24             	mov    %eax,(%esp)
  8000b1:	e8 72 07 00 00       	call   800828 <sys_getdirentry>
  8000b6:	85 c0                	test   %eax,%eax
  8000b8:	75 08                	jne    8000c2 <readdir+0x29>
        return &(dirp->dirent);
  8000ba:	8b 45 08             	mov    0x8(%ebp),%eax
  8000bd:	83 c0 04             	add    $0x4,%eax
  8000c0:	eb 05                	jmp    8000c7 <readdir+0x2e>
    }
    return NULL;
  8000c2:	b8 00 00 00 00       	mov    $0x0,%eax
}
  8000c7:	c9                   	leave  
  8000c8:	c3                   	ret    

008000c9 <closedir>:

void
closedir(DIR *dirp) {
  8000c9:	55                   	push   %ebp
  8000ca:	89 e5                	mov    %esp,%ebp
  8000cc:	83 ec 18             	sub    $0x18,%esp
    close(dirp->fd);
  8000cf:	8b 45 08             	mov    0x8(%ebp),%eax
  8000d2:	8b 00                	mov    (%eax),%eax
  8000d4:	89 04 24             	mov    %eax,(%esp)
  8000d7:	e8 76 00 00 00       	call   800152 <close>
}
  8000dc:	c9                   	leave  
  8000dd:	c3                   	ret    

008000de <mkdir>:

int
mkdir(const char *path) {
  8000de:	55                   	push   %ebp
  8000df:	89 e5                	mov    %esp,%ebp
  8000e1:	83 ec 18             	sub    $0x18,%esp
	return sys_mkdir(path);
  8000e4:	8b 45 08             	mov    0x8(%ebp),%eax
  8000e7:	89 04 24             	mov    %eax,(%esp)
  8000ea:	e8 1e 07 00 00       	call   80080d <sys_mkdir>
}
  8000ef:	c9                   	leave  
  8000f0:	c3                   	ret    

008000f1 <getcwd>:

int
getcwd(char *buffer, size_t len) {
  8000f1:	55                   	push   %ebp
  8000f2:	89 e5                	mov    %esp,%ebp
  8000f4:	83 ec 18             	sub    $0x18,%esp
    return sys_getcwd(buffer, len);
  8000f7:	8b 45 0c             	mov    0xc(%ebp),%eax
  8000fa:	89 44 24 04          	mov    %eax,0x4(%esp)
  8000fe:	8b 45 08             	mov    0x8(%ebp),%eax
  800101:	89 04 24             	mov    %eax,(%esp)
  800104:	e8 e2 06 00 00       	call   8007eb <sys_getcwd>
}
  800109:	c9                   	leave  
  80010a:	c3                   	ret    

0080010b <chcwd>:

int
chcwd(const char *path) {
  80010b:	55                   	push   %ebp
  80010c:	89 e5                	mov    %esp,%ebp
  80010e:	83 ec 18             	sub    $0x18,%esp
	return sys_chcwd(path);
  800111:	8b 45 08             	mov    0x8(%ebp),%eax
  800114:	89 04 24             	mov    %eax,(%esp)
  800117:	e8 bb 07 00 00       	call   8008d7 <sys_chcwd>
}
  80011c:	c9                   	leave  
  80011d:	c3                   	ret    

0080011e <open>:
#include <stat.h>
#include <error.h>
#include <unistd.h>

int
open(const char *path, uint32_t open_flags) {
  80011e:	55                   	push   %ebp
  80011f:	89 e5                	mov    %esp,%ebp
  800121:	83 ec 18             	sub    $0x18,%esp
    return sys_open(path, open_flags);
  800124:	8b 45 0c             	mov    0xc(%ebp),%eax
  800127:	89 44 24 04          	mov    %eax,0x4(%esp)
  80012b:	8b 45 08             	mov    0x8(%ebp),%eax
  80012e:	89 04 24             	mov    %eax,(%esp)
  800131:	e8 9e 05 00 00       	call   8006d4 <sys_open>
}
  800136:	c9                   	leave  
  800137:	c3                   	ret    

00800138 <delete>:

int
delete(const char *path, uint32_t type) {
  800138:	55                   	push   %ebp
  800139:	89 e5                	mov    %esp,%ebp
  80013b:	83 ec 18             	sub    $0x18,%esp
	return sys_delete(path, type);
  80013e:	8b 45 0c             	mov    0xc(%ebp),%eax
  800141:	89 44 24 04          	mov    %eax,0x4(%esp)
  800145:	8b 45 08             	mov    0x8(%ebp),%eax
  800148:	89 04 24             	mov    %eax,(%esp)
  80014b:	e8 a6 05 00 00       	call   8006f6 <sys_delete>
}
  800150:	c9                   	leave  
  800151:	c3                   	ret    

00800152 <close>:

int
close(int fd) {
  800152:	55                   	push   %ebp
  800153:	89 e5                	mov    %esp,%ebp
  800155:	83 ec 18             	sub    $0x18,%esp
    return sys_close(fd);
  800158:	8b 45 08             	mov    0x8(%ebp),%eax
  80015b:	89 04 24             	mov    %eax,(%esp)
  80015e:	e8 b5 05 00 00       	call   800718 <sys_close>
}
  800163:	c9                   	leave  
  800164:	c3                   	ret    

00800165 <read>:

int
read(int fd, void *base, size_t len) {
  800165:	55                   	push   %ebp
  800166:	89 e5                	mov    %esp,%ebp
  800168:	83 ec 18             	sub    $0x18,%esp
    return sys_read(fd, base, len);
  80016b:	8b 45 10             	mov    0x10(%ebp),%eax
  80016e:	89 44 24 08          	mov    %eax,0x8(%esp)
  800172:	8b 45 0c             	mov    0xc(%ebp),%eax
  800175:	89 44 24 04          	mov    %eax,0x4(%esp)
  800179:	8b 45 08             	mov    0x8(%ebp),%eax
  80017c:	89 04 24             	mov    %eax,(%esp)
  80017f:	e8 af 05 00 00       	call   800733 <sys_read>
}
  800184:	c9                   	leave  
  800185:	c3                   	ret    

00800186 <write>:

int
write(int fd, void *base, size_t len) {
  800186:	55                   	push   %ebp
  800187:	89 e5                	mov    %esp,%ebp
  800189:	83 ec 18             	sub    $0x18,%esp
    return sys_write(fd, base, len);
  80018c:	8b 45 10             	mov    0x10(%ebp),%eax
  80018f:	89 44 24 08          	mov    %eax,0x8(%esp)
  800193:	8b 45 0c             	mov    0xc(%ebp),%eax
  800196:	89 44 24 04          	mov    %eax,0x4(%esp)
  80019a:	8b 45 08             	mov    0x8(%ebp),%eax
  80019d:	89 04 24             	mov    %eax,(%esp)
  8001a0:	e8 b7 05 00 00       	call   80075c <sys_write>
}
  8001a5:	c9                   	leave  
  8001a6:	c3                   	ret    

008001a7 <seek>:

int
seek(int fd, off_t pos, int whence) {
  8001a7:	55                   	push   %ebp
  8001a8:	89 e5                	mov    %esp,%ebp
  8001aa:	83 ec 18             	sub    $0x18,%esp
    return sys_seek(fd, pos, whence);
  8001ad:	8b 45 10             	mov    0x10(%ebp),%eax
  8001b0:	89 44 24 08          	mov    %eax,0x8(%esp)
  8001b4:	8b 45 0c             	mov    0xc(%ebp),%eax
  8001b7:	89 44 24 04          	mov    %eax,0x4(%esp)
  8001bb:	8b 45 08             	mov    0x8(%ebp),%eax
  8001be:	89 04 24             	mov    %eax,(%esp)
  8001c1:	e8 bf 05 00 00       	call   800785 <sys_seek>
}
  8001c6:	c9                   	leave  
  8001c7:	c3                   	ret    

008001c8 <fstat>:

int
fstat(int fd, struct stat *stat) {
  8001c8:	55                   	push   %ebp
  8001c9:	89 e5                	mov    %esp,%ebp
  8001cb:	83 ec 18             	sub    $0x18,%esp
    return sys_fstat(fd, stat);
  8001ce:	8b 45 0c             	mov    0xc(%ebp),%eax
  8001d1:	89 44 24 04          	mov    %eax,0x4(%esp)
  8001d5:	8b 45 08             	mov    0x8(%ebp),%eax
  8001d8:	89 04 24             	mov    %eax,(%esp)
  8001db:	e8 ce 05 00 00       	call   8007ae <sys_fstat>
}
  8001e0:	c9                   	leave  
  8001e1:	c3                   	ret    

008001e2 <fsync>:

int
fsync(int fd) {
  8001e2:	55                   	push   %ebp
  8001e3:	89 e5                	mov    %esp,%ebp
  8001e5:	83 ec 18             	sub    $0x18,%esp
    return sys_fsync(fd);
  8001e8:	8b 45 08             	mov    0x8(%ebp),%eax
  8001eb:	89 04 24             	mov    %eax,(%esp)
  8001ee:	e8 dd 05 00 00       	call   8007d0 <sys_fsync>
}
  8001f3:	c9                   	leave  
  8001f4:	c3                   	ret    

008001f5 <dup2>:

int
dup2(int fd1, int fd2) {
  8001f5:	55                   	push   %ebp
  8001f6:	89 e5                	mov    %esp,%ebp
  8001f8:	83 ec 18             	sub    $0x18,%esp
    return sys_dup(fd1, fd2);
  8001fb:	8b 45 0c             	mov    0xc(%ebp),%eax
  8001fe:	89 44 24 04          	mov    %eax,0x4(%esp)
  800202:	8b 45 08             	mov    0x8(%ebp),%eax
  800205:	89 04 24             	mov    %eax,(%esp)
  800208:	e8 3d 06 00 00       	call   80084a <sys_dup>
}
  80020d:	c9                   	leave  
  80020e:	c3                   	ret    

0080020f <rename>:

int
rename(const char *oldName, const char *newName) {
  80020f:	55                   	push   %ebp
  800210:	89 e5                	mov    %esp,%ebp
  800212:	83 ec 18             	sub    $0x18,%esp
	return sys_rename(oldName, newName);
  800215:	8b 45 0c             	mov    0xc(%ebp),%eax
  800218:	89 44 24 04          	mov    %eax,0x4(%esp)
  80021c:	8b 45 08             	mov    0x8(%ebp),%eax
  80021f:	89 04 24             	mov    %eax,(%esp)
  800222:	e8 cb 06 00 00       	call   8008f2 <sys_rename>
}
  800227:	c9                   	leave  
  800228:	c3                   	ret    

00800229 <transmode>:

static char
transmode(struct stat *stat) {
  800229:	55                   	push   %ebp
  80022a:	89 e5                	mov    %esp,%ebp
  80022c:	83 ec 10             	sub    $0x10,%esp
    uint32_t mode = stat->st_mode;
  80022f:	8b 45 08             	mov    0x8(%ebp),%eax
  800232:	8b 00                	mov    (%eax),%eax
  800234:	89 45 fc             	mov    %eax,-0x4(%ebp)
    if (S_ISREG(mode)) return 'r';
  800237:	8b 45 fc             	mov    -0x4(%ebp),%eax
  80023a:	25 00 70 00 00       	and    $0x7000,%eax
  80023f:	3d 00 10 00 00       	cmp    $0x1000,%eax
  800244:	75 07                	jne    80024d <transmode+0x24>
  800246:	b8 72 00 00 00       	mov    $0x72,%eax
  80024b:	eb 5d                	jmp    8002aa <transmode+0x81>
    if (S_ISDIR(mode)) return 'd';
  80024d:	8b 45 fc             	mov    -0x4(%ebp),%eax
  800250:	25 00 70 00 00       	and    $0x7000,%eax
  800255:	3d 00 20 00 00       	cmp    $0x2000,%eax
  80025a:	75 07                	jne    800263 <transmode+0x3a>
  80025c:	b8 64 00 00 00       	mov    $0x64,%eax
  800261:	eb 47                	jmp    8002aa <transmode+0x81>
    if (S_ISLNK(mode)) return 'l';
  800263:	8b 45 fc             	mov    -0x4(%ebp),%eax
  800266:	25 00 70 00 00       	and    $0x7000,%eax
  80026b:	3d 00 30 00 00       	cmp    $0x3000,%eax
  800270:	75 07                	jne    800279 <transmode+0x50>
  800272:	b8 6c 00 00 00       	mov    $0x6c,%eax
  800277:	eb 31                	jmp    8002aa <transmode+0x81>
    if (S_ISCHR(mode)) return 'c';
  800279:	8b 45 fc             	mov    -0x4(%ebp),%eax
  80027c:	25 00 70 00 00       	and    $0x7000,%eax
  800281:	3d 00 40 00 00       	cmp    $0x4000,%eax
  800286:	75 07                	jne    80028f <transmode+0x66>
  800288:	b8 63 00 00 00       	mov    $0x63,%eax
  80028d:	eb 1b                	jmp    8002aa <transmode+0x81>
    if (S_ISBLK(mode)) return 'b';
  80028f:	8b 45 fc             	mov    -0x4(%ebp),%eax
  800292:	25 00 70 00 00       	and    $0x7000,%eax
  800297:	3d 00 50 00 00       	cmp    $0x5000,%eax
  80029c:	75 07                	jne    8002a5 <transmode+0x7c>
  80029e:	b8 62 00 00 00       	mov    $0x62,%eax
  8002a3:	eb 05                	jmp    8002aa <transmode+0x81>
    return '-';
  8002a5:	b8 2d 00 00 00       	mov    $0x2d,%eax
}
  8002aa:	c9                   	leave  
  8002ab:	c3                   	ret    

008002ac <print_stat>:

void
print_stat(const char *name, int fd, struct stat *stat) {
  8002ac:	55                   	push   %ebp
  8002ad:	89 e5                	mov    %esp,%ebp
  8002af:	83 ec 18             	sub    $0x18,%esp
    cprintf("[%03d] %s\n", fd, name);
  8002b2:	8b 45 08             	mov    0x8(%ebp),%eax
  8002b5:	89 44 24 08          	mov    %eax,0x8(%esp)
  8002b9:	8b 45 0c             	mov    0xc(%ebp),%eax
  8002bc:	89 44 24 04          	mov    %eax,0x4(%esp)
  8002c0:	c7 04 24 60 1a 80 00 	movl   $0x801a60,(%esp)
  8002c7:	e8 6b 01 00 00       	call   800437 <cprintf>
    cprintf("    mode    : %c\n", transmode(stat));
  8002cc:	8b 45 10             	mov    0x10(%ebp),%eax
  8002cf:	89 04 24             	mov    %eax,(%esp)
  8002d2:	e8 52 ff ff ff       	call   800229 <transmode>
  8002d7:	0f be c0             	movsbl %al,%eax
  8002da:	89 44 24 04          	mov    %eax,0x4(%esp)
  8002de:	c7 04 24 6b 1a 80 00 	movl   $0x801a6b,(%esp)
  8002e5:	e8 4d 01 00 00       	call   800437 <cprintf>
    cprintf("    links   : %lu\n", stat->st_nlinks);
  8002ea:	8b 45 10             	mov    0x10(%ebp),%eax
  8002ed:	8b 40 04             	mov    0x4(%eax),%eax
  8002f0:	89 44 24 04          	mov    %eax,0x4(%esp)
  8002f4:	c7 04 24 7d 1a 80 00 	movl   $0x801a7d,(%esp)
  8002fb:	e8 37 01 00 00       	call   800437 <cprintf>
    cprintf("    blocks  : %lu\n", stat->st_blocks);
  800300:	8b 45 10             	mov    0x10(%ebp),%eax
  800303:	8b 40 08             	mov    0x8(%eax),%eax
  800306:	89 44 24 04          	mov    %eax,0x4(%esp)
  80030a:	c7 04 24 90 1a 80 00 	movl   $0x801a90,(%esp)
  800311:	e8 21 01 00 00       	call   800437 <cprintf>
    cprintf("    size    : %lu\n", stat->st_size);
  800316:	8b 45 10             	mov    0x10(%ebp),%eax
  800319:	8b 40 0c             	mov    0xc(%eax),%eax
  80031c:	89 44 24 04          	mov    %eax,0x4(%esp)
  800320:	c7 04 24 a3 1a 80 00 	movl   $0x801aa3,(%esp)
  800327:	e8 0b 01 00 00       	call   800437 <cprintf>
}
  80032c:	c9                   	leave  
  80032d:	c3                   	ret    

0080032e <_start>:
.text
.globl _start
_start:
    # set ebp for backtrace
    movl $0x0, %ebp
  80032e:	bd 00 00 00 00       	mov    $0x0,%ebp

    # load argc and argv
    movl (%esp), %ebx
  800333:	8b 1c 24             	mov    (%esp),%ebx
    lea 0x4(%esp), %ecx
  800336:	8d 4c 24 04          	lea    0x4(%esp),%ecx


    # move down the esp register
    # since it may cause page fault in backtrace
    subl $0x20, %esp
  80033a:	83 ec 20             	sub    $0x20,%esp

    # save argc and argv on stack
    pushl %ecx
  80033d:	51                   	push   %ecx
    pushl %ebx
  80033e:	53                   	push   %ebx

    # call user-program function
    call umain
  80033f:	e8 53 09 00 00       	call   800c97 <umain>
1:  jmp 1b
  800344:	eb fe                	jmp    800344 <_start+0x16>

00800346 <__panic>:
#include <stdio.h>
#include <ulib.h>
#include <error.h>

void
__panic(const char *file, int line, const char *fmt, ...) {
  800346:	55                   	push   %ebp
  800347:	89 e5                	mov    %esp,%ebp
  800349:	83 ec 28             	sub    $0x28,%esp
    // print the 'message'
    va_list ap;
    va_start(ap, fmt);
  80034c:	8d 45 14             	lea    0x14(%ebp),%eax
  80034f:	89 45 f4             	mov    %eax,-0xc(%ebp)
    cprintf("user panic at %s:%d:\n    ", file, line);
  800352:	8b 45 0c             	mov    0xc(%ebp),%eax
  800355:	89 44 24 08          	mov    %eax,0x8(%esp)
  800359:	8b 45 08             	mov    0x8(%ebp),%eax
  80035c:	89 44 24 04          	mov    %eax,0x4(%esp)
  800360:	c7 04 24 b6 1a 80 00 	movl   $0x801ab6,(%esp)
  800367:	e8 cb 00 00 00       	call   800437 <cprintf>
    vcprintf(fmt, ap);
  80036c:	8b 45 f4             	mov    -0xc(%ebp),%eax
  80036f:	89 44 24 04          	mov    %eax,0x4(%esp)
  800373:	8b 45 10             	mov    0x10(%ebp),%eax
  800376:	89 04 24             	mov    %eax,(%esp)
  800379:	e8 7e 00 00 00       	call   8003fc <vcprintf>
    cprintf("\n");
  80037e:	c7 04 24 d0 1a 80 00 	movl   $0x801ad0,(%esp)
  800385:	e8 ad 00 00 00       	call   800437 <cprintf>
    va_end(ap);
    exit(-E_PANIC);
  80038a:	c7 04 24 f6 ff ff ff 	movl   $0xfffffff6,(%esp)
  800391:	e8 e3 06 00 00       	call   800a79 <exit>

00800396 <__warn>:
}

void
__warn(const char *file, int line, const char *fmt, ...) {
  800396:	55                   	push   %ebp
  800397:	89 e5                	mov    %esp,%ebp
  800399:	83 ec 28             	sub    $0x28,%esp
    va_list ap;
    va_start(ap, fmt);
  80039c:	8d 45 14             	lea    0x14(%ebp),%eax
  80039f:	89 45 f4             	mov    %eax,-0xc(%ebp)
    cprintf("user warning at %s:%d:\n    ", file, line);
  8003a2:	8b 45 0c             	mov    0xc(%ebp),%eax
  8003a5:	89 44 24 08          	mov    %eax,0x8(%esp)
  8003a9:	8b 45 08             	mov    0x8(%ebp),%eax
  8003ac:	89 44 24 04          	mov    %eax,0x4(%esp)
  8003b0:	c7 04 24 d2 1a 80 00 	movl   $0x801ad2,(%esp)
  8003b7:	e8 7b 00 00 00       	call   800437 <cprintf>
    vcprintf(fmt, ap);
  8003bc:	8b 45 f4             	mov    -0xc(%ebp),%eax
  8003bf:	89 44 24 04          	mov    %eax,0x4(%esp)
  8003c3:	8b 45 10             	mov    0x10(%ebp),%eax
  8003c6:	89 04 24             	mov    %eax,(%esp)
  8003c9:	e8 2e 00 00 00       	call   8003fc <vcprintf>
    cprintf("\n");
  8003ce:	c7 04 24 d0 1a 80 00 	movl   $0x801ad0,(%esp)
  8003d5:	e8 5d 00 00 00       	call   800437 <cprintf>
    va_end(ap);
}
  8003da:	c9                   	leave  
  8003db:	c3                   	ret    

008003dc <cputch>:
/* *
 * cputch - writes a single character @c to stdout, and it will
 * increace the value of counter pointed by @cnt.
 * */
static void
cputch(int c, int *cnt) {
  8003dc:	55                   	push   %ebp
  8003dd:	89 e5                	mov    %esp,%ebp
  8003df:	83 ec 18             	sub    $0x18,%esp
    sys_putc(c);
  8003e2:	8b 45 08             	mov    0x8(%ebp),%eax
  8003e5:	89 04 24             	mov    %eax,(%esp)
  8003e8:	e8 45 02 00 00       	call   800632 <sys_putc>
    (*cnt) ++;
  8003ed:	8b 45 0c             	mov    0xc(%ebp),%eax
  8003f0:	8b 00                	mov    (%eax),%eax
  8003f2:	8d 50 01             	lea    0x1(%eax),%edx
  8003f5:	8b 45 0c             	mov    0xc(%ebp),%eax
  8003f8:	89 10                	mov    %edx,(%eax)
}
  8003fa:	c9                   	leave  
  8003fb:	c3                   	ret    

008003fc <vcprintf>:
 *
 * Call this function if you are already dealing with a va_list.
 * Or you probably want cprintf() instead.
 * */
int
vcprintf(const char *fmt, va_list ap) {
  8003fc:	55                   	push   %ebp
  8003fd:	89 e5                	mov    %esp,%ebp
  8003ff:	83 ec 38             	sub    $0x38,%esp
    int cnt = 0;
  800402:	c7 45 f4 00 00 00 00 	movl   $0x0,-0xc(%ebp)
    vprintfmt((void*)cputch, NO_FD, &cnt, fmt, ap);
  800409:	8b 45 0c             	mov    0xc(%ebp),%eax
  80040c:	89 44 24 10          	mov    %eax,0x10(%esp)
  800410:	8b 45 08             	mov    0x8(%ebp),%eax
  800413:	89 44 24 0c          	mov    %eax,0xc(%esp)
  800417:	8d 45 f4             	lea    -0xc(%ebp),%eax
  80041a:	89 44 24 08          	mov    %eax,0x8(%esp)
  80041e:	c7 44 24 04 d9 6a ff 	movl   $0xffff6ad9,0x4(%esp)
  800425:	ff 
  800426:	c7 04 24 dc 03 80 00 	movl   $0x8003dc,(%esp)
  80042d:	e8 25 0b 00 00       	call   800f57 <vprintfmt>
    return cnt;
  800432:	8b 45 f4             	mov    -0xc(%ebp),%eax
}
  800435:	c9                   	leave  
  800436:	c3                   	ret    

00800437 <cprintf>:
 *
 * The return value is the number of characters which would be
 * written to stdout.
 * */
int
cprintf(const char *fmt, ...) {
  800437:	55                   	push   %ebp
  800438:	89 e5                	mov    %esp,%ebp
  80043a:	83 ec 28             	sub    $0x28,%esp
    va_list ap;

    va_start(ap, fmt);
  80043d:	8d 45 0c             	lea    0xc(%ebp),%eax
  800440:	89 45 f0             	mov    %eax,-0x10(%ebp)
    int cnt = vcprintf(fmt, ap);
  800443:	8b 45 f0             	mov    -0x10(%ebp),%eax
  800446:	89 44 24 04          	mov    %eax,0x4(%esp)
  80044a:	8b 45 08             	mov    0x8(%ebp),%eax
  80044d:	89 04 24             	mov    %eax,(%esp)
  800450:	e8 a7 ff ff ff       	call   8003fc <vcprintf>
  800455:	89 45 f4             	mov    %eax,-0xc(%ebp)
    va_end(ap);

    return cnt;
  800458:	8b 45 f4             	mov    -0xc(%ebp),%eax
}
  80045b:	c9                   	leave  
  80045c:	c3                   	ret    

0080045d <cputs>:
/* *
 * cputs- writes the string pointed by @str to stdout and
 * appends a newline character.
 * */
int
cputs(const char *str) {
  80045d:	55                   	push   %ebp
  80045e:	89 e5                	mov    %esp,%ebp
  800460:	83 ec 28             	sub    $0x28,%esp
    int cnt = 0;
  800463:	c7 45 f0 00 00 00 00 	movl   $0x0,-0x10(%ebp)
    char c;
    while ((c = *str ++) != '\0') {
  80046a:	eb 13                	jmp    80047f <cputs+0x22>
        cputch(c, &cnt);
  80046c:	0f be 45 f7          	movsbl -0x9(%ebp),%eax
  800470:	8d 55 f0             	lea    -0x10(%ebp),%edx
  800473:	89 54 24 04          	mov    %edx,0x4(%esp)
  800477:	89 04 24             	mov    %eax,(%esp)
  80047a:	e8 5d ff ff ff       	call   8003dc <cputch>
 * */
int
cputs(const char *str) {
    int cnt = 0;
    char c;
    while ((c = *str ++) != '\0') {
  80047f:	8b 45 08             	mov    0x8(%ebp),%eax
  800482:	8d 50 01             	lea    0x1(%eax),%edx
  800485:	89 55 08             	mov    %edx,0x8(%ebp)
  800488:	0f b6 00             	movzbl (%eax),%eax
  80048b:	88 45 f7             	mov    %al,-0x9(%ebp)
  80048e:	80 7d f7 00          	cmpb   $0x0,-0x9(%ebp)
  800492:	75 d8                	jne    80046c <cputs+0xf>
        cputch(c, &cnt);
    }
    cputch('\n', &cnt);
  800494:	8d 45 f0             	lea    -0x10(%ebp),%eax
  800497:	89 44 24 04          	mov    %eax,0x4(%esp)
  80049b:	c7 04 24 0a 00 00 00 	movl   $0xa,(%esp)
  8004a2:	e8 35 ff ff ff       	call   8003dc <cputch>
    return cnt;
  8004a7:	8b 45 f0             	mov    -0x10(%ebp),%eax
}
  8004aa:	c9                   	leave  
  8004ab:	c3                   	ret    

008004ac <fputch>:


static void
fputch(char c, int *cnt, int fd) {
  8004ac:	55                   	push   %ebp
  8004ad:	89 e5                	mov    %esp,%ebp
  8004af:	83 ec 18             	sub    $0x18,%esp
  8004b2:	8b 45 08             	mov    0x8(%ebp),%eax
  8004b5:	88 45 f4             	mov    %al,-0xc(%ebp)
    write(fd, &c, sizeof(char));
  8004b8:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
  8004bf:	00 
  8004c0:	8d 45 f4             	lea    -0xc(%ebp),%eax
  8004c3:	89 44 24 04          	mov    %eax,0x4(%esp)
  8004c7:	8b 45 10             	mov    0x10(%ebp),%eax
  8004ca:	89 04 24             	mov    %eax,(%esp)
  8004cd:	e8 b4 fc ff ff       	call   800186 <write>
    (*cnt) ++;
  8004d2:	8b 45 0c             	mov    0xc(%ebp),%eax
  8004d5:	8b 00                	mov    (%eax),%eax
  8004d7:	8d 50 01             	lea    0x1(%eax),%edx
  8004da:	8b 45 0c             	mov    0xc(%ebp),%eax
  8004dd:	89 10                	mov    %edx,(%eax)
}
  8004df:	c9                   	leave  
  8004e0:	c3                   	ret    

008004e1 <vfprintf>:

int
vfprintf(int fd, const char *fmt, va_list ap) {
  8004e1:	55                   	push   %ebp
  8004e2:	89 e5                	mov    %esp,%ebp
  8004e4:	83 ec 38             	sub    $0x38,%esp
    int cnt = 0;
  8004e7:	c7 45 f4 00 00 00 00 	movl   $0x0,-0xc(%ebp)
    vprintfmt((void*)fputch, fd, &cnt, fmt, ap);
  8004ee:	8b 45 10             	mov    0x10(%ebp),%eax
  8004f1:	89 44 24 10          	mov    %eax,0x10(%esp)
  8004f5:	8b 45 0c             	mov    0xc(%ebp),%eax
  8004f8:	89 44 24 0c          	mov    %eax,0xc(%esp)
  8004fc:	8d 45 f4             	lea    -0xc(%ebp),%eax
  8004ff:	89 44 24 08          	mov    %eax,0x8(%esp)
  800503:	8b 45 08             	mov    0x8(%ebp),%eax
  800506:	89 44 24 04          	mov    %eax,0x4(%esp)
  80050a:	c7 04 24 ac 04 80 00 	movl   $0x8004ac,(%esp)
  800511:	e8 41 0a 00 00       	call   800f57 <vprintfmt>
    return cnt;
  800516:	8b 45 f4             	mov    -0xc(%ebp),%eax
}
  800519:	c9                   	leave  
  80051a:	c3                   	ret    

0080051b <fprintf>:

int
fprintf(int fd, const char *fmt, ...) {
  80051b:	55                   	push   %ebp
  80051c:	89 e5                	mov    %esp,%ebp
  80051e:	83 ec 28             	sub    $0x28,%esp
    va_list ap;

    va_start(ap, fmt);
  800521:	8d 45 10             	lea    0x10(%ebp),%eax
  800524:	89 45 f0             	mov    %eax,-0x10(%ebp)
    int cnt = vfprintf(fd, fmt, ap);
  800527:	8b 45 f0             	mov    -0x10(%ebp),%eax
  80052a:	89 44 24 08          	mov    %eax,0x8(%esp)
  80052e:	8b 45 0c             	mov    0xc(%ebp),%eax
  800531:	89 44 24 04          	mov    %eax,0x4(%esp)
  800535:	8b 45 08             	mov    0x8(%ebp),%eax
  800538:	89 04 24             	mov    %eax,(%esp)
  80053b:	e8 a1 ff ff ff       	call   8004e1 <vfprintf>
  800540:	89 45 f4             	mov    %eax,-0xc(%ebp)
    va_end(ap);

    return cnt;
  800543:	8b 45 f4             	mov    -0xc(%ebp),%eax
}
  800546:	c9                   	leave  
  800547:	c3                   	ret    

00800548 <syscall>:


#define MAX_ARGS            5

static inline int
syscall(int num, ...) {
  800548:	55                   	push   %ebp
  800549:	89 e5                	mov    %esp,%ebp
  80054b:	57                   	push   %edi
  80054c:	56                   	push   %esi
  80054d:	53                   	push   %ebx
  80054e:	83 ec 20             	sub    $0x20,%esp
    va_list ap;
    va_start(ap, num);
  800551:	8d 45 0c             	lea    0xc(%ebp),%eax
  800554:	89 45 e8             	mov    %eax,-0x18(%ebp)
    uint32_t a[MAX_ARGS];
    int i, ret;
    for (i = 0; i < MAX_ARGS; i ++) {
  800557:	c7 45 f0 00 00 00 00 	movl   $0x0,-0x10(%ebp)
  80055e:	eb 16                	jmp    800576 <syscall+0x2e>
        a[i] = va_arg(ap, uint32_t);
  800560:	8b 45 e8             	mov    -0x18(%ebp),%eax
  800563:	8d 50 04             	lea    0x4(%eax),%edx
  800566:	89 55 e8             	mov    %edx,-0x18(%ebp)
  800569:	8b 10                	mov    (%eax),%edx
  80056b:	8b 45 f0             	mov    -0x10(%ebp),%eax
  80056e:	89 54 85 d4          	mov    %edx,-0x2c(%ebp,%eax,4)
syscall(int num, ...) {
    va_list ap;
    va_start(ap, num);
    uint32_t a[MAX_ARGS];
    int i, ret;
    for (i = 0; i < MAX_ARGS; i ++) {
  800572:	83 45 f0 01          	addl   $0x1,-0x10(%ebp)
  800576:	83 7d f0 04          	cmpl   $0x4,-0x10(%ebp)
  80057a:	7e e4                	jle    800560 <syscall+0x18>
    asm volatile (
        "int %1;"
        : "=a" (ret)
        : "i" (T_SYSCALL),
          "a" (num),
          "d" (a[0]),
  80057c:	8b 55 d4             	mov    -0x2c(%ebp),%edx
          "c" (a[1]),
  80057f:	8b 4d d8             	mov    -0x28(%ebp),%ecx
          "b" (a[2]),
  800582:	8b 5d dc             	mov    -0x24(%ebp),%ebx
          "D" (a[3]),
  800585:	8b 7d e0             	mov    -0x20(%ebp),%edi
          "S" (a[4])
  800588:	8b 75 e4             	mov    -0x1c(%ebp),%esi
    for (i = 0; i < MAX_ARGS; i ++) {
        a[i] = va_arg(ap, uint32_t);
    }
    va_end(ap);

    asm volatile (
  80058b:	8b 45 08             	mov    0x8(%ebp),%eax
  80058e:	cd 80                	int    $0x80
  800590:	89 45 ec             	mov    %eax,-0x14(%ebp)
          "c" (a[1]),
          "b" (a[2]),
          "D" (a[3]),
          "S" (a[4])
        : "cc", "memory");
    return ret;
  800593:	8b 45 ec             	mov    -0x14(%ebp),%eax
}
  800596:	83 c4 20             	add    $0x20,%esp
  800599:	5b                   	pop    %ebx
  80059a:	5e                   	pop    %esi
  80059b:	5f                   	pop    %edi
  80059c:	5d                   	pop    %ebp
  80059d:	c3                   	ret    

0080059e <sys_exit>:

int
sys_exit(int error_code) {
  80059e:	55                   	push   %ebp
  80059f:	89 e5                	mov    %esp,%ebp
  8005a1:	83 ec 08             	sub    $0x8,%esp
    return syscall(SYS_exit, error_code);
  8005a4:	8b 45 08             	mov    0x8(%ebp),%eax
  8005a7:	89 44 24 04          	mov    %eax,0x4(%esp)
  8005ab:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
  8005b2:	e8 91 ff ff ff       	call   800548 <syscall>
}
  8005b7:	c9                   	leave  
  8005b8:	c3                   	ret    

008005b9 <sys_fork>:

int
sys_fork(void) {
  8005b9:	55                   	push   %ebp
  8005ba:	89 e5                	mov    %esp,%ebp
  8005bc:	83 ec 04             	sub    $0x4,%esp
    return syscall(SYS_fork);
  8005bf:	c7 04 24 02 00 00 00 	movl   $0x2,(%esp)
  8005c6:	e8 7d ff ff ff       	call   800548 <syscall>
}
  8005cb:	c9                   	leave  
  8005cc:	c3                   	ret    

008005cd <sys_wait>:

int
sys_wait(int pid, int *store) {
  8005cd:	55                   	push   %ebp
  8005ce:	89 e5                	mov    %esp,%ebp
  8005d0:	83 ec 0c             	sub    $0xc,%esp
    return syscall(SYS_wait, pid, store);
  8005d3:	8b 45 0c             	mov    0xc(%ebp),%eax
  8005d6:	89 44 24 08          	mov    %eax,0x8(%esp)
  8005da:	8b 45 08             	mov    0x8(%ebp),%eax
  8005dd:	89 44 24 04          	mov    %eax,0x4(%esp)
  8005e1:	c7 04 24 03 00 00 00 	movl   $0x3,(%esp)
  8005e8:	e8 5b ff ff ff       	call   800548 <syscall>
}
  8005ed:	c9                   	leave  
  8005ee:	c3                   	ret    

008005ef <sys_yield>:

int
sys_yield(void) {
  8005ef:	55                   	push   %ebp
  8005f0:	89 e5                	mov    %esp,%ebp
  8005f2:	83 ec 04             	sub    $0x4,%esp
    return syscall(SYS_yield);
  8005f5:	c7 04 24 0a 00 00 00 	movl   $0xa,(%esp)
  8005fc:	e8 47 ff ff ff       	call   800548 <syscall>
}
  800601:	c9                   	leave  
  800602:	c3                   	ret    

00800603 <sys_kill>:

int
sys_kill(int pid) {
  800603:	55                   	push   %ebp
  800604:	89 e5                	mov    %esp,%ebp
  800606:	83 ec 08             	sub    $0x8,%esp
    return syscall(SYS_kill, pid);
  800609:	8b 45 08             	mov    0x8(%ebp),%eax
  80060c:	89 44 24 04          	mov    %eax,0x4(%esp)
  800610:	c7 04 24 0c 00 00 00 	movl   $0xc,(%esp)
  800617:	e8 2c ff ff ff       	call   800548 <syscall>
}
  80061c:	c9                   	leave  
  80061d:	c3                   	ret    

0080061e <sys_getpid>:

int
sys_getpid(void) {
  80061e:	55                   	push   %ebp
  80061f:	89 e5                	mov    %esp,%ebp
  800621:	83 ec 04             	sub    $0x4,%esp
    return syscall(SYS_getpid);
  800624:	c7 04 24 12 00 00 00 	movl   $0x12,(%esp)
  80062b:	e8 18 ff ff ff       	call   800548 <syscall>
}
  800630:	c9                   	leave  
  800631:	c3                   	ret    

00800632 <sys_putc>:

int
sys_putc(int c) {
  800632:	55                   	push   %ebp
  800633:	89 e5                	mov    %esp,%ebp
  800635:	83 ec 08             	sub    $0x8,%esp
    return syscall(SYS_putc, c);
  800638:	8b 45 08             	mov    0x8(%ebp),%eax
  80063b:	89 44 24 04          	mov    %eax,0x4(%esp)
  80063f:	c7 04 24 1e 00 00 00 	movl   $0x1e,(%esp)
  800646:	e8 fd fe ff ff       	call   800548 <syscall>
}
  80064b:	c9                   	leave  
  80064c:	c3                   	ret    

0080064d <sys_pgdir>:

int
sys_pgdir(void) {
  80064d:	55                   	push   %ebp
  80064e:	89 e5                	mov    %esp,%ebp
  800650:	83 ec 04             	sub    $0x4,%esp
    return syscall(SYS_pgdir);
  800653:	c7 04 24 1f 00 00 00 	movl   $0x1f,(%esp)
  80065a:	e8 e9 fe ff ff       	call   800548 <syscall>
}
  80065f:	c9                   	leave  
  800660:	c3                   	ret    

00800661 <sys_lab6_set_priority>:

void
sys_lab6_set_priority(uint32_t priority)
{
  800661:	55                   	push   %ebp
  800662:	89 e5                	mov    %esp,%ebp
  800664:	83 ec 08             	sub    $0x8,%esp
    syscall(SYS_lab6_set_priority, priority);
  800667:	8b 45 08             	mov    0x8(%ebp),%eax
  80066a:	89 44 24 04          	mov    %eax,0x4(%esp)
  80066e:	c7 04 24 ff 00 00 00 	movl   $0xff,(%esp)
  800675:	e8 ce fe ff ff       	call   800548 <syscall>
}
  80067a:	c9                   	leave  
  80067b:	c3                   	ret    

0080067c <sys_sleep>:

int
sys_sleep(unsigned int time) {
  80067c:	55                   	push   %ebp
  80067d:	89 e5                	mov    %esp,%ebp
  80067f:	83 ec 08             	sub    $0x8,%esp
    return syscall(SYS_sleep, time);
  800682:	8b 45 08             	mov    0x8(%ebp),%eax
  800685:	89 44 24 04          	mov    %eax,0x4(%esp)
  800689:	c7 04 24 0b 00 00 00 	movl   $0xb,(%esp)
  800690:	e8 b3 fe ff ff       	call   800548 <syscall>
}
  800695:	c9                   	leave  
  800696:	c3                   	ret    

00800697 <sys_gettime>:

size_t
sys_gettime(void) {
  800697:	55                   	push   %ebp
  800698:	89 e5                	mov    %esp,%ebp
  80069a:	83 ec 04             	sub    $0x4,%esp
    return syscall(SYS_gettime);
  80069d:	c7 04 24 11 00 00 00 	movl   $0x11,(%esp)
  8006a4:	e8 9f fe ff ff       	call   800548 <syscall>
}
  8006a9:	c9                   	leave  
  8006aa:	c3                   	ret    

008006ab <sys_exec>:

int
sys_exec(const char *name, int argc, const char **argv) {
  8006ab:	55                   	push   %ebp
  8006ac:	89 e5                	mov    %esp,%ebp
  8006ae:	83 ec 10             	sub    $0x10,%esp
    return syscall(SYS_exec, name, argc, argv);
  8006b1:	8b 45 10             	mov    0x10(%ebp),%eax
  8006b4:	89 44 24 0c          	mov    %eax,0xc(%esp)
  8006b8:	8b 45 0c             	mov    0xc(%ebp),%eax
  8006bb:	89 44 24 08          	mov    %eax,0x8(%esp)
  8006bf:	8b 45 08             	mov    0x8(%ebp),%eax
  8006c2:	89 44 24 04          	mov    %eax,0x4(%esp)
  8006c6:	c7 04 24 04 00 00 00 	movl   $0x4,(%esp)
  8006cd:	e8 76 fe ff ff       	call   800548 <syscall>
}
  8006d2:	c9                   	leave  
  8006d3:	c3                   	ret    

008006d4 <sys_open>:

int
sys_open(const char *path, uint32_t open_flags) {
  8006d4:	55                   	push   %ebp
  8006d5:	89 e5                	mov    %esp,%ebp
  8006d7:	83 ec 0c             	sub    $0xc,%esp
    return syscall(SYS_open, path, open_flags);
  8006da:	8b 45 0c             	mov    0xc(%ebp),%eax
  8006dd:	89 44 24 08          	mov    %eax,0x8(%esp)
  8006e1:	8b 45 08             	mov    0x8(%ebp),%eax
  8006e4:	89 44 24 04          	mov    %eax,0x4(%esp)
  8006e8:	c7 04 24 64 00 00 00 	movl   $0x64,(%esp)
  8006ef:	e8 54 fe ff ff       	call   800548 <syscall>
}
  8006f4:	c9                   	leave  
  8006f5:	c3                   	ret    

008006f6 <sys_delete>:

int
sys_delete(const char *path, uint32_t type) {
  8006f6:	55                   	push   %ebp
  8006f7:	89 e5                	mov    %esp,%ebp
  8006f9:	83 ec 0c             	sub    $0xc,%esp
	return syscall(SYS_delete, path, type);
  8006fc:	8b 45 0c             	mov    0xc(%ebp),%eax
  8006ff:	89 44 24 08          	mov    %eax,0x8(%esp)
  800703:	8b 45 08             	mov    0x8(%ebp),%eax
  800706:	89 44 24 04          	mov    %eax,0x4(%esp)
  80070a:	c7 04 24 69 00 00 00 	movl   $0x69,(%esp)
  800711:	e8 32 fe ff ff       	call   800548 <syscall>
}
  800716:	c9                   	leave  
  800717:	c3                   	ret    

00800718 <sys_close>:

int
sys_close(int fd) {
  800718:	55                   	push   %ebp
  800719:	89 e5                	mov    %esp,%ebp
  80071b:	83 ec 08             	sub    $0x8,%esp
    return syscall(SYS_close, fd);
  80071e:	8b 45 08             	mov    0x8(%ebp),%eax
  800721:	89 44 24 04          	mov    %eax,0x4(%esp)
  800725:	c7 04 24 65 00 00 00 	movl   $0x65,(%esp)
  80072c:	e8 17 fe ff ff       	call   800548 <syscall>
}
  800731:	c9                   	leave  
  800732:	c3                   	ret    

00800733 <sys_read>:

int
sys_read(int fd, void *base, size_t len) {
  800733:	55                   	push   %ebp
  800734:	89 e5                	mov    %esp,%ebp
  800736:	83 ec 10             	sub    $0x10,%esp
    return syscall(SYS_read, fd, base, len);
  800739:	8b 45 10             	mov    0x10(%ebp),%eax
  80073c:	89 44 24 0c          	mov    %eax,0xc(%esp)
  800740:	8b 45 0c             	mov    0xc(%ebp),%eax
  800743:	89 44 24 08          	mov    %eax,0x8(%esp)
  800747:	8b 45 08             	mov    0x8(%ebp),%eax
  80074a:	89 44 24 04          	mov    %eax,0x4(%esp)
  80074e:	c7 04 24 66 00 00 00 	movl   $0x66,(%esp)
  800755:	e8 ee fd ff ff       	call   800548 <syscall>
}
  80075a:	c9                   	leave  
  80075b:	c3                   	ret    

0080075c <sys_write>:

int
sys_write(int fd, void *base, size_t len) {
  80075c:	55                   	push   %ebp
  80075d:	89 e5                	mov    %esp,%ebp
  80075f:	83 ec 10             	sub    $0x10,%esp
    return syscall(SYS_write, fd, base, len);
  800762:	8b 45 10             	mov    0x10(%ebp),%eax
  800765:	89 44 24 0c          	mov    %eax,0xc(%esp)
  800769:	8b 45 0c             	mov    0xc(%ebp),%eax
  80076c:	89 44 24 08          	mov    %eax,0x8(%esp)
  800770:	8b 45 08             	mov    0x8(%ebp),%eax
  800773:	89 44 24 04          	mov    %eax,0x4(%esp)
  800777:	c7 04 24 67 00 00 00 	movl   $0x67,(%esp)
  80077e:	e8 c5 fd ff ff       	call   800548 <syscall>
}
  800783:	c9                   	leave  
  800784:	c3                   	ret    

00800785 <sys_seek>:

int
sys_seek(int fd, off_t pos, int whence) {
  800785:	55                   	push   %ebp
  800786:	89 e5                	mov    %esp,%ebp
  800788:	83 ec 10             	sub    $0x10,%esp
    return syscall(SYS_seek, fd, pos, whence);
  80078b:	8b 45 10             	mov    0x10(%ebp),%eax
  80078e:	89 44 24 0c          	mov    %eax,0xc(%esp)
  800792:	8b 45 0c             	mov    0xc(%ebp),%eax
  800795:	89 44 24 08          	mov    %eax,0x8(%esp)
  800799:	8b 45 08             	mov    0x8(%ebp),%eax
  80079c:	89 44 24 04          	mov    %eax,0x4(%esp)
  8007a0:	c7 04 24 68 00 00 00 	movl   $0x68,(%esp)
  8007a7:	e8 9c fd ff ff       	call   800548 <syscall>
}
  8007ac:	c9                   	leave  
  8007ad:	c3                   	ret    

008007ae <sys_fstat>:

int
sys_fstat(int fd, struct stat *stat) {
  8007ae:	55                   	push   %ebp
  8007af:	89 e5                	mov    %esp,%ebp
  8007b1:	83 ec 0c             	sub    $0xc,%esp
    return syscall(SYS_fstat, fd, stat);
  8007b4:	8b 45 0c             	mov    0xc(%ebp),%eax
  8007b7:	89 44 24 08          	mov    %eax,0x8(%esp)
  8007bb:	8b 45 08             	mov    0x8(%ebp),%eax
  8007be:	89 44 24 04          	mov    %eax,0x4(%esp)
  8007c2:	c7 04 24 6e 00 00 00 	movl   $0x6e,(%esp)
  8007c9:	e8 7a fd ff ff       	call   800548 <syscall>
}
  8007ce:	c9                   	leave  
  8007cf:	c3                   	ret    

008007d0 <sys_fsync>:

int
sys_fsync(int fd) {
  8007d0:	55                   	push   %ebp
  8007d1:	89 e5                	mov    %esp,%ebp
  8007d3:	83 ec 08             	sub    $0x8,%esp
    return syscall(SYS_fsync, fd);
  8007d6:	8b 45 08             	mov    0x8(%ebp),%eax
  8007d9:	89 44 24 04          	mov    %eax,0x4(%esp)
  8007dd:	c7 04 24 6f 00 00 00 	movl   $0x6f,(%esp)
  8007e4:	e8 5f fd ff ff       	call   800548 <syscall>
}
  8007e9:	c9                   	leave  
  8007ea:	c3                   	ret    

008007eb <sys_getcwd>:

int
sys_getcwd(char *buffer, size_t len) {
  8007eb:	55                   	push   %ebp
  8007ec:	89 e5                	mov    %esp,%ebp
  8007ee:	83 ec 0c             	sub    $0xc,%esp
    return syscall(SYS_getcwd, buffer, len);
  8007f1:	8b 45 0c             	mov    0xc(%ebp),%eax
  8007f4:	89 44 24 08          	mov    %eax,0x8(%esp)
  8007f8:	8b 45 08             	mov    0x8(%ebp),%eax
  8007fb:	89 44 24 04          	mov    %eax,0x4(%esp)
  8007ff:	c7 04 24 79 00 00 00 	movl   $0x79,(%esp)
  800806:	e8 3d fd ff ff       	call   800548 <syscall>
}
  80080b:	c9                   	leave  
  80080c:	c3                   	ret    

0080080d <sys_mkdir>:

int
sys_mkdir(const char *path) {
  80080d:	55                   	push   %ebp
  80080e:	89 e5                	mov    %esp,%ebp
  800810:	83 ec 08             	sub    $0x8,%esp
	return syscall(SYS_mkdir, path);
  800813:	8b 45 08             	mov    0x8(%ebp),%eax
  800816:	89 44 24 04          	mov    %eax,0x4(%esp)
  80081a:	c7 04 24 7a 00 00 00 	movl   $0x7a,(%esp)
  800821:	e8 22 fd ff ff       	call   800548 <syscall>
}
  800826:	c9                   	leave  
  800827:	c3                   	ret    

00800828 <sys_getdirentry>:

int
sys_getdirentry(int fd, struct dirent *dirent) {
  800828:	55                   	push   %ebp
  800829:	89 e5                	mov    %esp,%ebp
  80082b:	83 ec 0c             	sub    $0xc,%esp
    return syscall(SYS_getdirentry, fd, dirent);
  80082e:	8b 45 0c             	mov    0xc(%ebp),%eax
  800831:	89 44 24 08          	mov    %eax,0x8(%esp)
  800835:	8b 45 08             	mov    0x8(%ebp),%eax
  800838:	89 44 24 04          	mov    %eax,0x4(%esp)
  80083c:	c7 04 24 80 00 00 00 	movl   $0x80,(%esp)
  800843:	e8 00 fd ff ff       	call   800548 <syscall>
}
  800848:	c9                   	leave  
  800849:	c3                   	ret    

0080084a <sys_dup>:

int
sys_dup(int fd1, int fd2) {
  80084a:	55                   	push   %ebp
  80084b:	89 e5                	mov    %esp,%ebp
  80084d:	83 ec 0c             	sub    $0xc,%esp
    return syscall(SYS_dup, fd1, fd2);
  800850:	8b 45 0c             	mov    0xc(%ebp),%eax
  800853:	89 44 24 08          	mov    %eax,0x8(%esp)
  800857:	8b 45 08             	mov    0x8(%ebp),%eax
  80085a:	89 44 24 04          	mov    %eax,0x4(%esp)
  80085e:	c7 04 24 82 00 00 00 	movl   $0x82,(%esp)
  800865:	e8 de fc ff ff       	call   800548 <syscall>
}
  80086a:	c9                   	leave  
  80086b:	c3                   	ret    

0080086c <sys_malloc>:

int
sys_malloc(int n){
  80086c:	55                   	push   %ebp
  80086d:	89 e5                	mov    %esp,%ebp
  80086f:	83 ec 08             	sub    $0x8,%esp
    return syscall(SYS_malloc, n);
  800872:	8b 45 08             	mov    0x8(%ebp),%eax
  800875:	89 44 24 04          	mov    %eax,0x4(%esp)
  800879:	c7 04 24 83 00 00 00 	movl   $0x83,(%esp)
  800880:	e8 c3 fc ff ff       	call   800548 <syscall>
}
  800885:	c9                   	leave  
  800886:	c3                   	ret    

00800887 <sys_queue>:

void
sys_queue(void){
  800887:	55                   	push   %ebp
  800888:	89 e5                	mov    %esp,%ebp
  80088a:	83 ec 04             	sub    $0x4,%esp
    syscall(SYS_queue);
  80088d:	c7 04 24 96 00 00 00 	movl   $0x96,(%esp)
  800894:	e8 af fc ff ff       	call   800548 <syscall>
}
  800899:	c9                   	leave  
  80089a:	c3                   	ret    

0080089b <sys_listProc>:

void
sys_listProc(void){
  80089b:	55                   	push   %ebp
  80089c:	89 e5                	mov    %esp,%ebp
  80089e:	83 ec 04             	sub    $0x4,%esp
    syscall(SYS_listProc);
  8008a1:	c7 04 24 8b 00 00 00 	movl   $0x8b,(%esp)
  8008a8:	e8 9b fc ff ff       	call   800548 <syscall>
}
  8008ad:	c9                   	leave  
  8008ae:	c3                   	ret    

008008af <sys_current_vm>:

void
sys_current_vm(void){
  8008af:	55                   	push   %ebp
  8008b0:	89 e5                	mov    %esp,%ebp
  8008b2:	83 ec 04             	sub    $0x4,%esp
    syscall(SYS_current_vm);
  8008b5:	c7 04 24 8c 00 00 00 	movl   $0x8c,(%esp)
  8008bc:	e8 87 fc ff ff       	call   800548 <syscall>
}
  8008c1:	c9                   	leave  
  8008c2:	c3                   	ret    

008008c3 <sys_mem>:

void
sys_mem(void){
  8008c3:	55                   	push   %ebp
  8008c4:	89 e5                	mov    %esp,%ebp
  8008c6:	83 ec 04             	sub    $0x4,%esp
    syscall(SYS_mem);
  8008c9:	c7 04 24 84 00 00 00 	movl   $0x84,(%esp)
  8008d0:	e8 73 fc ff ff       	call   800548 <syscall>
}
  8008d5:	c9                   	leave  
  8008d6:	c3                   	ret    

008008d7 <sys_chcwd>:

int
sys_chcwd(const char *path) {
  8008d7:	55                   	push   %ebp
  8008d8:	89 e5                	mov    %esp,%ebp
  8008da:	83 ec 08             	sub    $0x8,%esp
	return syscall(SYS_chcwd, path);
  8008dd:	8b 45 08             	mov    0x8(%ebp),%eax
  8008e0:	89 44 24 04          	mov    %eax,0x4(%esp)
  8008e4:	c7 04 24 7b 00 00 00 	movl   $0x7b,(%esp)
  8008eb:	e8 58 fc ff ff       	call   800548 <syscall>
}
  8008f0:	c9                   	leave  
  8008f1:	c3                   	ret    

008008f2 <sys_rename>:

int
sys_rename(const char *oldName, const char *newName) {
  8008f2:	55                   	push   %ebp
  8008f3:	89 e5                	mov    %esp,%ebp
  8008f5:	83 ec 0c             	sub    $0xc,%esp
	return syscall(SYS_rename, oldName, newName);
  8008f8:	8b 45 0c             	mov    0xc(%ebp),%eax
  8008fb:	89 44 24 08          	mov    %eax,0x8(%esp)
  8008ff:	8b 45 08             	mov    0x8(%ebp),%eax
  800902:	89 44 24 04          	mov    %eax,0x4(%esp)
  800906:	c7 04 24 85 00 00 00 	movl   $0x85,(%esp)
  80090d:	e8 36 fc ff ff       	call   800548 <syscall>
}
  800912:	c9                   	leave  
  800913:	c3                   	ret    

00800914 <sys_pmm_check>:

void sys_pmm_check(void){
  800914:	55                   	push   %ebp
  800915:	89 e5                	mov    %esp,%ebp
  800917:	83 ec 04             	sub    $0x4,%esp
	syscall(SYS_check_pmm);
  80091a:	c7 04 24 86 00 00 00 	movl   $0x86,(%esp)
  800921:	e8 22 fc ff ff       	call   800548 <syscall>
}
  800926:	c9                   	leave  
  800927:	c3                   	ret    

00800928 <sys_swap_check>:

void sys_swap_check(void){
  800928:	55                   	push   %ebp
  800929:	89 e5                	mov    %esp,%ebp
  80092b:	83 ec 04             	sub    $0x4,%esp
	syscall(SYS_check_swap);
  80092e:	c7 04 24 87 00 00 00 	movl   $0x87,(%esp)
  800935:	e8 0e fc ff ff       	call   800548 <syscall>
}
  80093a:	c9                   	leave  
  80093b:	c3                   	ret    

0080093c <sys_sched_check>:

void sys_sched_check(void){
  80093c:	55                   	push   %ebp
  80093d:	89 e5                	mov    %esp,%ebp
  80093f:	83 ec 04             	sub    $0x4,%esp
	syscall(SYS_check_sched);
  800942:	c7 04 24 88 00 00 00 	movl   $0x88,(%esp)
  800949:	e8 fa fb ff ff       	call   800548 <syscall>
}
  80094e:	c9                   	leave  
  80094f:	c3                   	ret    

00800950 <sys_change_swap>:

void 
sys_change_swap(unsigned int mode){
  800950:	55                   	push   %ebp
  800951:	89 e5                	mov    %esp,%ebp
  800953:	83 ec 08             	sub    $0x8,%esp
    syscall(SYS_change_swap,mode);
  800956:	8b 45 08             	mov    0x8(%ebp),%eax
  800959:	89 44 24 04          	mov    %eax,0x4(%esp)
  80095d:	c7 04 24 89 00 00 00 	movl   $0x89,(%esp)
  800964:	e8 df fb ff ff       	call   800548 <syscall>
}
  800969:	c9                   	leave  
  80096a:	c3                   	ret    

0080096b <sys_change_sched>:

void 
sys_change_sched(unsigned int mode){
  80096b:	55                   	push   %ebp
  80096c:	89 e5                	mov    %esp,%ebp
  80096e:	83 ec 08             	sub    $0x8,%esp
    syscall(SYS_change_sched,mode);
  800971:	8b 45 08             	mov    0x8(%ebp),%eax
  800974:	89 44 24 04          	mov    %eax,0x4(%esp)
  800978:	c7 04 24 98 00 00 00 	movl   $0x98,(%esp)
  80097f:	e8 c4 fb ff ff       	call   800548 <syscall>
}
  800984:	c9                   	leave  
  800985:	c3                   	ret    

00800986 <sys_stride_queue>:

void
sys_stride_queue(void){
  800986:	55                   	push   %ebp
  800987:	89 e5                	mov    %esp,%ebp
  800989:	83 ec 04             	sub    $0x4,%esp
    syscall(SYS_stride_queue);
  80098c:	c7 04 24 99 00 00 00 	movl   $0x99,(%esp)
  800993:	e8 b0 fb ff ff       	call   800548 <syscall>
}
  800998:	c9                   	leave  
  800999:	c3                   	ret    

0080099a <sys_list_wait>:

void
sys_list_wait(void){
  80099a:	55                   	push   %ebp
  80099b:	89 e5                	mov    %esp,%ebp
  80099d:	83 ec 04             	sub    $0x4,%esp
    syscall(SYS_list_wait);
  8009a0:	c7 04 24 9a 00 00 00 	movl   $0x9a,(%esp)
  8009a7:	e8 9c fb ff ff       	call   800548 <syscall>
}
  8009ac:	c9                   	leave  
  8009ad:	c3                   	ret    

008009ae <try_lock>:
lock_init(lock_t *l) {
    *l = 0;
}

static inline bool
try_lock(lock_t *l) {
  8009ae:	55                   	push   %ebp
  8009af:	89 e5                	mov    %esp,%ebp
  8009b1:	83 ec 10             	sub    $0x10,%esp
  8009b4:	c7 45 fc 00 00 00 00 	movl   $0x0,-0x4(%ebp)
  8009bb:	8b 45 08             	mov    0x8(%ebp),%eax
  8009be:	89 45 f8             	mov    %eax,-0x8(%ebp)
 * @addr:   the address to count from
 * */
static inline bool
test_and_set_bit(int nr, volatile void *addr) {
    int oldbit;
    asm volatile ("btsl %2, %1; sbbl %0, %0" : "=r" (oldbit), "=m" (*(volatile long *)addr) : "Ir" (nr) : "memory");
  8009c1:	8b 55 f8             	mov    -0x8(%ebp),%edx
  8009c4:	8b 45 fc             	mov    -0x4(%ebp),%eax
  8009c7:	0f ab 02             	bts    %eax,(%edx)
  8009ca:	19 c0                	sbb    %eax,%eax
  8009cc:	89 45 f4             	mov    %eax,-0xc(%ebp)
    return oldbit != 0;
  8009cf:	83 7d f4 00          	cmpl   $0x0,-0xc(%ebp)
  8009d3:	0f 95 c0             	setne  %al
  8009d6:	0f b6 c0             	movzbl %al,%eax
    return test_and_set_bit(0, l);
}
  8009d9:	c9                   	leave  
  8009da:	c3                   	ret    

008009db <lock>:

static inline void
lock(lock_t *l) {
  8009db:	55                   	push   %ebp
  8009dc:	89 e5                	mov    %esp,%ebp
  8009de:	83 ec 28             	sub    $0x28,%esp
    if (try_lock(l)) {
  8009e1:	8b 45 08             	mov    0x8(%ebp),%eax
  8009e4:	89 04 24             	mov    %eax,(%esp)
  8009e7:	e8 c2 ff ff ff       	call   8009ae <try_lock>
  8009ec:	85 c0                	test   %eax,%eax
  8009ee:	74 38                	je     800a28 <lock+0x4d>
        int step = 0;
  8009f0:	c7 45 f4 00 00 00 00 	movl   $0x0,-0xc(%ebp)
        do {
            yield();
  8009f7:	e8 df 00 00 00       	call   800adb <yield>
            if (++ step == 100) {
  8009fc:	83 45 f4 01          	addl   $0x1,-0xc(%ebp)
  800a00:	83 7d f4 64          	cmpl   $0x64,-0xc(%ebp)
  800a04:	75 13                	jne    800a19 <lock+0x3e>
                step = 0;
  800a06:	c7 45 f4 00 00 00 00 	movl   $0x0,-0xc(%ebp)
                sleep(10);
  800a0d:	c7 04 24 0a 00 00 00 	movl   $0xa,(%esp)
  800a14:	e8 0f 01 00 00       	call   800b28 <sleep>
            }
        } while (try_lock(l));
  800a19:	8b 45 08             	mov    0x8(%ebp),%eax
  800a1c:	89 04 24             	mov    %eax,(%esp)
  800a1f:	e8 8a ff ff ff       	call   8009ae <try_lock>
  800a24:	85 c0                	test   %eax,%eax
  800a26:	75 cf                	jne    8009f7 <lock+0x1c>
    }
}
  800a28:	c9                   	leave  
  800a29:	c3                   	ret    

00800a2a <unlock>:

static inline void
unlock(lock_t *l) {
  800a2a:	55                   	push   %ebp
  800a2b:	89 e5                	mov    %esp,%ebp
  800a2d:	83 ec 10             	sub    $0x10,%esp
  800a30:	c7 45 fc 00 00 00 00 	movl   $0x0,-0x4(%ebp)
  800a37:	8b 45 08             	mov    0x8(%ebp),%eax
  800a3a:	89 45 f8             	mov    %eax,-0x8(%ebp)
 * @addr:   the address to count from
 * */
static inline bool
test_and_clear_bit(int nr, volatile void *addr) {
    int oldbit;
    asm volatile ("btrl %2, %1; sbbl %0, %0" : "=r" (oldbit), "=m" (*(volatile long *)addr) : "Ir" (nr) : "memory");
  800a3d:	8b 55 f8             	mov    -0x8(%ebp),%edx
  800a40:	8b 45 fc             	mov    -0x4(%ebp),%eax
  800a43:	0f b3 02             	btr    %eax,(%edx)
  800a46:	19 c0                	sbb    %eax,%eax
  800a48:	89 45 f4             	mov    %eax,-0xc(%ebp)
    return oldbit != 0;
  800a4b:	83 7d f4 00          	cmpl   $0x0,-0xc(%ebp)
    test_and_clear_bit(0, l);
}
  800a4f:	c9                   	leave  
  800a50:	c3                   	ret    

00800a51 <lock_fork>:
#include <lock.h>

static lock_t fork_lock = INIT_LOCK;

void
lock_fork(void) {
  800a51:	55                   	push   %ebp
  800a52:	89 e5                	mov    %esp,%ebp
  800a54:	83 ec 18             	sub    $0x18,%esp
    lock(&fork_lock);
  800a57:	c7 04 24 20 20 80 00 	movl   $0x802020,(%esp)
  800a5e:	e8 78 ff ff ff       	call   8009db <lock>
}
  800a63:	c9                   	leave  
  800a64:	c3                   	ret    

00800a65 <unlock_fork>:

void
unlock_fork(void) {
  800a65:	55                   	push   %ebp
  800a66:	89 e5                	mov    %esp,%ebp
  800a68:	83 ec 04             	sub    $0x4,%esp
    unlock(&fork_lock);
  800a6b:	c7 04 24 20 20 80 00 	movl   $0x802020,(%esp)
  800a72:	e8 b3 ff ff ff       	call   800a2a <unlock>
}
  800a77:	c9                   	leave  
  800a78:	c3                   	ret    

00800a79 <exit>:

void
exit(int error_code) {
  800a79:	55                   	push   %ebp
  800a7a:	89 e5                	mov    %esp,%ebp
  800a7c:	83 ec 18             	sub    $0x18,%esp
    sys_exit(error_code);
  800a7f:	8b 45 08             	mov    0x8(%ebp),%eax
  800a82:	89 04 24             	mov    %eax,(%esp)
  800a85:	e8 14 fb ff ff       	call   80059e <sys_exit>
    cprintf("BUG: exit failed.\n");
  800a8a:	c7 04 24 ee 1a 80 00 	movl   $0x801aee,(%esp)
  800a91:	e8 a1 f9 ff ff       	call   800437 <cprintf>
    while (1);
  800a96:	eb fe                	jmp    800a96 <exit+0x1d>

00800a98 <fork>:
}

int
fork(void) {
  800a98:	55                   	push   %ebp
  800a99:	89 e5                	mov    %esp,%ebp
  800a9b:	83 ec 08             	sub    $0x8,%esp
    return sys_fork();
  800a9e:	e8 16 fb ff ff       	call   8005b9 <sys_fork>
}
  800aa3:	c9                   	leave  
  800aa4:	c3                   	ret    

00800aa5 <wait>:

int
wait(void) {
  800aa5:	55                   	push   %ebp
  800aa6:	89 e5                	mov    %esp,%ebp
  800aa8:	83 ec 18             	sub    $0x18,%esp
    return sys_wait(0, NULL);
  800aab:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
  800ab2:	00 
  800ab3:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
  800aba:	e8 0e fb ff ff       	call   8005cd <sys_wait>
}
  800abf:	c9                   	leave  
  800ac0:	c3                   	ret    

00800ac1 <waitpid>:

int
waitpid(int pid, int *store) {
  800ac1:	55                   	push   %ebp
  800ac2:	89 e5                	mov    %esp,%ebp
  800ac4:	83 ec 18             	sub    $0x18,%esp
    return sys_wait(pid, store);
  800ac7:	8b 45 0c             	mov    0xc(%ebp),%eax
  800aca:	89 44 24 04          	mov    %eax,0x4(%esp)
  800ace:	8b 45 08             	mov    0x8(%ebp),%eax
  800ad1:	89 04 24             	mov    %eax,(%esp)
  800ad4:	e8 f4 fa ff ff       	call   8005cd <sys_wait>
}
  800ad9:	c9                   	leave  
  800ada:	c3                   	ret    

00800adb <yield>:

void
yield(void) {
  800adb:	55                   	push   %ebp
  800adc:	89 e5                	mov    %esp,%ebp
  800ade:	83 ec 08             	sub    $0x8,%esp
    sys_yield();
  800ae1:	e8 09 fb ff ff       	call   8005ef <sys_yield>
}
  800ae6:	c9                   	leave  
  800ae7:	c3                   	ret    

00800ae8 <kill>:

int
kill(int pid) {
  800ae8:	55                   	push   %ebp
  800ae9:	89 e5                	mov    %esp,%ebp
  800aeb:	83 ec 18             	sub    $0x18,%esp
    return sys_kill(pid);
  800aee:	8b 45 08             	mov    0x8(%ebp),%eax
  800af1:	89 04 24             	mov    %eax,(%esp)
  800af4:	e8 0a fb ff ff       	call   800603 <sys_kill>
}
  800af9:	c9                   	leave  
  800afa:	c3                   	ret    

00800afb <getpid>:

int
getpid(void) {
  800afb:	55                   	push   %ebp
  800afc:	89 e5                	mov    %esp,%ebp
  800afe:	83 ec 08             	sub    $0x8,%esp
    return sys_getpid();
  800b01:	e8 18 fb ff ff       	call   80061e <sys_getpid>
}
  800b06:	c9                   	leave  
  800b07:	c3                   	ret    

00800b08 <print_pgdir>:

//print_pgdir - print the PDT&PT
void
print_pgdir(void) {
  800b08:	55                   	push   %ebp
  800b09:	89 e5                	mov    %esp,%ebp
  800b0b:	83 ec 08             	sub    $0x8,%esp
    sys_pgdir();
  800b0e:	e8 3a fb ff ff       	call   80064d <sys_pgdir>
}
  800b13:	c9                   	leave  
  800b14:	c3                   	ret    

00800b15 <lab6_set_priority>:

void
lab6_set_priority(uint32_t priority)
{
  800b15:	55                   	push   %ebp
  800b16:	89 e5                	mov    %esp,%ebp
  800b18:	83 ec 18             	sub    $0x18,%esp
    sys_lab6_set_priority(priority);
  800b1b:	8b 45 08             	mov    0x8(%ebp),%eax
  800b1e:	89 04 24             	mov    %eax,(%esp)
  800b21:	e8 3b fb ff ff       	call   800661 <sys_lab6_set_priority>
}
  800b26:	c9                   	leave  
  800b27:	c3                   	ret    

00800b28 <sleep>:

int
sleep(unsigned int time) {
  800b28:	55                   	push   %ebp
  800b29:	89 e5                	mov    %esp,%ebp
  800b2b:	83 ec 18             	sub    $0x18,%esp
    return sys_sleep(time);
  800b2e:	8b 45 08             	mov    0x8(%ebp),%eax
  800b31:	89 04 24             	mov    %eax,(%esp)
  800b34:	e8 43 fb ff ff       	call   80067c <sys_sleep>
}
  800b39:	c9                   	leave  
  800b3a:	c3                   	ret    

00800b3b <gettime_msec>:

unsigned int
gettime_msec(void) {
  800b3b:	55                   	push   %ebp
  800b3c:	89 e5                	mov    %esp,%ebp
  800b3e:	83 ec 08             	sub    $0x8,%esp
    return (unsigned int)sys_gettime();
  800b41:	e8 51 fb ff ff       	call   800697 <sys_gettime>
}
  800b46:	c9                   	leave  
  800b47:	c3                   	ret    

00800b48 <__exec>:

int
__exec(const char *name, const char **argv) {
  800b48:	55                   	push   %ebp
  800b49:	89 e5                	mov    %esp,%ebp
  800b4b:	83 ec 28             	sub    $0x28,%esp
    int argc = 0;
  800b4e:	c7 45 f4 00 00 00 00 	movl   $0x0,-0xc(%ebp)
    while (argv[argc] != NULL) {
  800b55:	eb 04                	jmp    800b5b <__exec+0x13>
        argc ++;
  800b57:	83 45 f4 01          	addl   $0x1,-0xc(%ebp)
}

int
__exec(const char *name, const char **argv) {
    int argc = 0;
    while (argv[argc] != NULL) {
  800b5b:	8b 45 f4             	mov    -0xc(%ebp),%eax
  800b5e:	8d 14 85 00 00 00 00 	lea    0x0(,%eax,4),%edx
  800b65:	8b 45 0c             	mov    0xc(%ebp),%eax
  800b68:	01 d0                	add    %edx,%eax
  800b6a:	8b 00                	mov    (%eax),%eax
  800b6c:	85 c0                	test   %eax,%eax
  800b6e:	75 e7                	jne    800b57 <__exec+0xf>
        argc ++;
    }
    return sys_exec(name, argc, argv);
  800b70:	8b 45 0c             	mov    0xc(%ebp),%eax
  800b73:	89 44 24 08          	mov    %eax,0x8(%esp)
  800b77:	8b 45 f4             	mov    -0xc(%ebp),%eax
  800b7a:	89 44 24 04          	mov    %eax,0x4(%esp)
  800b7e:	8b 45 08             	mov    0x8(%ebp),%eax
  800b81:	89 04 24             	mov    %eax,(%esp)
  800b84:	e8 22 fb ff ff       	call   8006ab <sys_exec>
}
  800b89:	c9                   	leave  
  800b8a:	c3                   	ret    

00800b8b <malloc>:

int malloc(int n){
  800b8b:	55                   	push   %ebp
  800b8c:	89 e5                	mov    %esp,%ebp
  800b8e:	83 ec 18             	sub    $0x18,%esp
    return sys_malloc(n);
  800b91:	8b 45 08             	mov    0x8(%ebp),%eax
  800b94:	89 04 24             	mov    %eax,(%esp)
  800b97:	e8 d0 fc ff ff       	call   80086c <sys_malloc>
}
  800b9c:	c9                   	leave  
  800b9d:	c3                   	ret    

00800b9e <getrunqueue>:

void getrunqueue(void){
  800b9e:	55                   	push   %ebp
  800b9f:	89 e5                	mov    %esp,%ebp
  800ba1:	83 ec 08             	sub    $0x8,%esp
    sys_queue();
  800ba4:	e8 de fc ff ff       	call   800887 <sys_queue>
}
  800ba9:	c9                   	leave  
  800baa:	c3                   	ret    

00800bab <listProc>:

void
listProc(void){
  800bab:	55                   	push   %ebp
  800bac:	89 e5                	mov    %esp,%ebp
  800bae:	83 ec 08             	sub    $0x8,%esp
    sys_listProc();
  800bb1:	e8 e5 fc ff ff       	call   80089b <sys_listProc>
} 
  800bb6:	c9                   	leave  
  800bb7:	c3                   	ret    

00800bb8 <current_vm>:

void
current_vm(void){
  800bb8:	55                   	push   %ebp
  800bb9:	89 e5                	mov    %esp,%ebp
  800bbb:	83 ec 08             	sub    $0x8,%esp
    sys_current_vm();
  800bbe:	e8 ec fc ff ff       	call   8008af <sys_current_vm>
}
  800bc3:	c9                   	leave  
  800bc4:	c3                   	ret    

00800bc5 <mem>:

void
mem(void){
  800bc5:	55                   	push   %ebp
  800bc6:	89 e5                	mov    %esp,%ebp
  800bc8:	83 ec 08             	sub    $0x8,%esp
    sys_mem();
  800bcb:	e8 f3 fc ff ff       	call   8008c3 <sys_mem>
}
  800bd0:	c9                   	leave  
  800bd1:	c3                   	ret    

00800bd2 <pmmcheck>:

void
pmmcheck(void){
  800bd2:	55                   	push   %ebp
  800bd3:	89 e5                	mov    %esp,%ebp
  800bd5:	83 ec 08             	sub    $0x8,%esp
     sys_pmm_check();
  800bd8:	e8 37 fd ff ff       	call   800914 <sys_pmm_check>
}
  800bdd:	c9                   	leave  
  800bde:	c3                   	ret    

00800bdf <swapcheck>:

void
swapcheck(void){
  800bdf:	55                   	push   %ebp
  800be0:	89 e5                	mov    %esp,%ebp
  800be2:	83 ec 08             	sub    $0x8,%esp
     sys_swap_check();
  800be5:	e8 3e fd ff ff       	call   800928 <sys_swap_check>
}
  800bea:	c9                   	leave  
  800beb:	c3                   	ret    

00800bec <schedcheck>:

void
schedcheck(void){
  800bec:	55                   	push   %ebp
  800bed:	89 e5                	mov    %esp,%ebp
  800bef:	83 ec 08             	sub    $0x8,%esp
     sys_sched_check();
  800bf2:	e8 45 fd ff ff       	call   80093c <sys_sched_check>
}
  800bf7:	c9                   	leave  
  800bf8:	c3                   	ret    

00800bf9 <change_swap>:

void 
change_swap(unsigned int mode){
  800bf9:	55                   	push   %ebp
  800bfa:	89 e5                	mov    %esp,%ebp
  800bfc:	83 ec 18             	sub    $0x18,%esp
    sys_change_swap(mode);
  800bff:	8b 45 08             	mov    0x8(%ebp),%eax
  800c02:	89 04 24             	mov    %eax,(%esp)
  800c05:	e8 46 fd ff ff       	call   800950 <sys_change_swap>
}
  800c0a:	c9                   	leave  
  800c0b:	c3                   	ret    

00800c0c <change_sched>:

void
change_sched(unsigned int mode){
  800c0c:	55                   	push   %ebp
  800c0d:	89 e5                	mov    %esp,%ebp
  800c0f:	83 ec 18             	sub    $0x18,%esp
    sys_change_sched(mode);
  800c12:	8b 45 08             	mov    0x8(%ebp),%eax
  800c15:	89 04 24             	mov    %eax,(%esp)
  800c18:	e8 4e fd ff ff       	call   80096b <sys_change_sched>
}
  800c1d:	c9                   	leave  
  800c1e:	c3                   	ret    

00800c1f <stride_queue>:

void stride_queue(void){
  800c1f:	55                   	push   %ebp
  800c20:	89 e5                	mov    %esp,%ebp
  800c22:	83 ec 08             	sub    $0x8,%esp
    sys_stride_queue();
  800c25:	e8 5c fd ff ff       	call   800986 <sys_stride_queue>
}
  800c2a:	c9                   	leave  
  800c2b:	c3                   	ret    

00800c2c <listwait>:

void
listwait(void){
  800c2c:	55                   	push   %ebp
  800c2d:	89 e5                	mov    %esp,%ebp
  800c2f:	83 ec 08             	sub    $0x8,%esp
    sys_list_wait();
  800c32:	e8 63 fd ff ff       	call   80099a <sys_list_wait>
}
  800c37:	c9                   	leave  
  800c38:	c3                   	ret    

00800c39 <initfd>:
#include <stat.h>

int main(int argc, char *argv[]);

static int
initfd(int fd2, const char *path, uint32_t open_flags) {
  800c39:	55                   	push   %ebp
  800c3a:	89 e5                	mov    %esp,%ebp
  800c3c:	83 ec 28             	sub    $0x28,%esp
    int fd1, ret;
    if ((fd1 = open(path, open_flags)) < 0) {
  800c3f:	8b 45 10             	mov    0x10(%ebp),%eax
  800c42:	89 44 24 04          	mov    %eax,0x4(%esp)
  800c46:	8b 45 0c             	mov    0xc(%ebp),%eax
  800c49:	89 04 24             	mov    %eax,(%esp)
  800c4c:	e8 cd f4 ff ff       	call   80011e <open>
  800c51:	89 45 f0             	mov    %eax,-0x10(%ebp)
  800c54:	83 7d f0 00          	cmpl   $0x0,-0x10(%ebp)
  800c58:	79 05                	jns    800c5f <initfd+0x26>
        return fd1;
  800c5a:	8b 45 f0             	mov    -0x10(%ebp),%eax
  800c5d:	eb 36                	jmp    800c95 <initfd+0x5c>
    }
    if (fd1 != fd2) {
  800c5f:	8b 45 f0             	mov    -0x10(%ebp),%eax
  800c62:	3b 45 08             	cmp    0x8(%ebp),%eax
  800c65:	74 2b                	je     800c92 <initfd+0x59>
        close(fd2);
  800c67:	8b 45 08             	mov    0x8(%ebp),%eax
  800c6a:	89 04 24             	mov    %eax,(%esp)
  800c6d:	e8 e0 f4 ff ff       	call   800152 <close>
        ret = dup2(fd1, fd2);
  800c72:	8b 45 08             	mov    0x8(%ebp),%eax
  800c75:	89 44 24 04          	mov    %eax,0x4(%esp)
  800c79:	8b 45 f0             	mov    -0x10(%ebp),%eax
  800c7c:	89 04 24             	mov    %eax,(%esp)
  800c7f:	e8 71 f5 ff ff       	call   8001f5 <dup2>
  800c84:	89 45 f4             	mov    %eax,-0xc(%ebp)
        close(fd1);
  800c87:	8b 45 f0             	mov    -0x10(%ebp),%eax
  800c8a:	89 04 24             	mov    %eax,(%esp)
  800c8d:	e8 c0 f4 ff ff       	call   800152 <close>
    }
    return ret;
  800c92:	8b 45 f4             	mov    -0xc(%ebp),%eax
}
  800c95:	c9                   	leave  
  800c96:	c3                   	ret    

00800c97 <umain>:

void
umain(int argc, char *argv[]) {
  800c97:	55                   	push   %ebp
  800c98:	89 e5                	mov    %esp,%ebp
  800c9a:	83 ec 28             	sub    $0x28,%esp
    int fd;
    if ((fd = initfd(0, "stdin:", O_RDONLY)) < 0) {
  800c9d:	c7 44 24 08 00 00 00 	movl   $0x0,0x8(%esp)
  800ca4:	00 
  800ca5:	c7 44 24 04 01 1b 80 	movl   $0x801b01,0x4(%esp)
  800cac:	00 
  800cad:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
  800cb4:	e8 80 ff ff ff       	call   800c39 <initfd>
  800cb9:	89 45 f4             	mov    %eax,-0xc(%ebp)
  800cbc:	83 7d f4 00          	cmpl   $0x0,-0xc(%ebp)
  800cc0:	79 23                	jns    800ce5 <umain+0x4e>
        warn("open <stdin> failed: %e.\n", fd);
  800cc2:	8b 45 f4             	mov    -0xc(%ebp),%eax
  800cc5:	89 44 24 0c          	mov    %eax,0xc(%esp)
  800cc9:	c7 44 24 08 08 1b 80 	movl   $0x801b08,0x8(%esp)
  800cd0:	00 
  800cd1:	c7 44 24 04 1a 00 00 	movl   $0x1a,0x4(%esp)
  800cd8:	00 
  800cd9:	c7 04 24 22 1b 80 00 	movl   $0x801b22,(%esp)
  800ce0:	e8 b1 f6 ff ff       	call   800396 <__warn>
    }
    if ((fd = initfd(1, "stdout:", O_WRONLY)) < 0) {
  800ce5:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
  800cec:	00 
  800ced:	c7 44 24 04 34 1b 80 	movl   $0x801b34,0x4(%esp)
  800cf4:	00 
  800cf5:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
  800cfc:	e8 38 ff ff ff       	call   800c39 <initfd>
  800d01:	89 45 f4             	mov    %eax,-0xc(%ebp)
  800d04:	83 7d f4 00          	cmpl   $0x0,-0xc(%ebp)
  800d08:	79 23                	jns    800d2d <umain+0x96>
        warn("open <stdout> failed: %e.\n", fd);
  800d0a:	8b 45 f4             	mov    -0xc(%ebp),%eax
  800d0d:	89 44 24 0c          	mov    %eax,0xc(%esp)
  800d11:	c7 44 24 08 3c 1b 80 	movl   $0x801b3c,0x8(%esp)
  800d18:	00 
  800d19:	c7 44 24 04 1d 00 00 	movl   $0x1d,0x4(%esp)
  800d20:	00 
  800d21:	c7 04 24 22 1b 80 00 	movl   $0x801b22,(%esp)
  800d28:	e8 69 f6 ff ff       	call   800396 <__warn>
    }
    int ret = main(argc, argv);
  800d2d:	8b 45 0c             	mov    0xc(%ebp),%eax
  800d30:	89 44 24 04          	mov    %eax,0x4(%esp)
  800d34:	8b 45 08             	mov    0x8(%ebp),%eax
  800d37:	89 04 24             	mov    %eax,(%esp)
  800d3a:	e8 f1 0c 00 00       	call   801a30 <main>
  800d3f:	89 45 f0             	mov    %eax,-0x10(%ebp)
    exit(ret);
  800d42:	8b 45 f0             	mov    -0x10(%ebp),%eax
  800d45:	89 04 24             	mov    %eax,(%esp)
  800d48:	e8 2c fd ff ff       	call   800a79 <exit>

00800d4d <hash32>:
 * @bits:   the number of bits in a return value
 *
 * High bits are more random, so we use them.
 * */
uint32_t
hash32(uint32_t val, unsigned int bits) {
  800d4d:	55                   	push   %ebp
  800d4e:	89 e5                	mov    %esp,%ebp
  800d50:	83 ec 10             	sub    $0x10,%esp
    uint32_t hash = val * GOLDEN_RATIO_PRIME_32;
  800d53:	8b 45 08             	mov    0x8(%ebp),%eax
  800d56:	69 c0 01 00 37 9e    	imul   $0x9e370001,%eax,%eax
  800d5c:	89 45 fc             	mov    %eax,-0x4(%ebp)
    return (hash >> (32 - bits));
  800d5f:	b8 20 00 00 00       	mov    $0x20,%eax
  800d64:	2b 45 0c             	sub    0xc(%ebp),%eax
  800d67:	8b 55 fc             	mov    -0x4(%ebp),%edx
  800d6a:	89 c1                	mov    %eax,%ecx
  800d6c:	d3 ea                	shr    %cl,%edx
  800d6e:	89 d0                	mov    %edx,%eax
}
  800d70:	c9                   	leave  
  800d71:	c3                   	ret    

00800d72 <printnum>:
 * @width:      maximum number of digits, if the actual width is less than @width, use @padc instead
 * @padc:       character that padded on the left if the actual width is less than @width
 * */
static void
printnum(void (*putch)(int, void*, int), int fd, void *putdat,
        unsigned long long num, unsigned base, int width, int padc) {
  800d72:	55                   	push   %ebp
  800d73:	89 e5                	mov    %esp,%ebp
  800d75:	83 ec 58             	sub    $0x58,%esp
  800d78:	8b 45 14             	mov    0x14(%ebp),%eax
  800d7b:	89 45 d0             	mov    %eax,-0x30(%ebp)
  800d7e:	8b 45 18             	mov    0x18(%ebp),%eax
  800d81:	89 45 d4             	mov    %eax,-0x2c(%ebp)
    unsigned long long result = num;
  800d84:	8b 45 d0             	mov    -0x30(%ebp),%eax
  800d87:	8b 55 d4             	mov    -0x2c(%ebp),%edx
  800d8a:	89 45 e8             	mov    %eax,-0x18(%ebp)
  800d8d:	89 55 ec             	mov    %edx,-0x14(%ebp)
    unsigned mod = do_div(result, base);
  800d90:	8b 45 1c             	mov    0x1c(%ebp),%eax
  800d93:	89 45 e4             	mov    %eax,-0x1c(%ebp)
  800d96:	8b 45 e8             	mov    -0x18(%ebp),%eax
  800d99:	8b 55 ec             	mov    -0x14(%ebp),%edx
  800d9c:	89 45 e0             	mov    %eax,-0x20(%ebp)
  800d9f:	89 55 f0             	mov    %edx,-0x10(%ebp)
  800da2:	8b 45 f0             	mov    -0x10(%ebp),%eax
  800da5:	89 45 f4             	mov    %eax,-0xc(%ebp)
  800da8:	83 7d f0 00          	cmpl   $0x0,-0x10(%ebp)
  800dac:	74 1c                	je     800dca <printnum+0x58>
  800dae:	8b 45 f0             	mov    -0x10(%ebp),%eax
  800db1:	ba 00 00 00 00       	mov    $0x0,%edx
  800db6:	f7 75 e4             	divl   -0x1c(%ebp)
  800db9:	89 55 f4             	mov    %edx,-0xc(%ebp)
  800dbc:	8b 45 f0             	mov    -0x10(%ebp),%eax
  800dbf:	ba 00 00 00 00       	mov    $0x0,%edx
  800dc4:	f7 75 e4             	divl   -0x1c(%ebp)
  800dc7:	89 45 f0             	mov    %eax,-0x10(%ebp)
  800dca:	8b 45 e0             	mov    -0x20(%ebp),%eax
  800dcd:	8b 55 f4             	mov    -0xc(%ebp),%edx
  800dd0:	f7 75 e4             	divl   -0x1c(%ebp)
  800dd3:	89 45 e0             	mov    %eax,-0x20(%ebp)
  800dd6:	89 55 dc             	mov    %edx,-0x24(%ebp)
  800dd9:	8b 45 e0             	mov    -0x20(%ebp),%eax
  800ddc:	8b 55 f0             	mov    -0x10(%ebp),%edx
  800ddf:	89 45 e8             	mov    %eax,-0x18(%ebp)
  800de2:	89 55 ec             	mov    %edx,-0x14(%ebp)
  800de5:	8b 45 dc             	mov    -0x24(%ebp),%eax
  800de8:	89 45 d8             	mov    %eax,-0x28(%ebp)

    // first recursively print all preceding (more significant) digits
    if (num >= base) {
  800deb:	8b 45 1c             	mov    0x1c(%ebp),%eax
  800dee:	ba 00 00 00 00       	mov    $0x0,%edx
  800df3:	3b 55 d4             	cmp    -0x2c(%ebp),%edx
  800df6:	77 64                	ja     800e5c <printnum+0xea>
  800df8:	3b 55 d4             	cmp    -0x2c(%ebp),%edx
  800dfb:	72 05                	jb     800e02 <printnum+0x90>
  800dfd:	3b 45 d0             	cmp    -0x30(%ebp),%eax
  800e00:	77 5a                	ja     800e5c <printnum+0xea>
        printnum(putch, fd, putdat, result, base, width - 1, padc);
  800e02:	8b 45 20             	mov    0x20(%ebp),%eax
  800e05:	8d 50 ff             	lea    -0x1(%eax),%edx
  800e08:	8b 45 24             	mov    0x24(%ebp),%eax
  800e0b:	89 44 24 1c          	mov    %eax,0x1c(%esp)
  800e0f:	89 54 24 18          	mov    %edx,0x18(%esp)
  800e13:	8b 45 1c             	mov    0x1c(%ebp),%eax
  800e16:	89 44 24 14          	mov    %eax,0x14(%esp)
  800e1a:	8b 45 e8             	mov    -0x18(%ebp),%eax
  800e1d:	8b 55 ec             	mov    -0x14(%ebp),%edx
  800e20:	89 44 24 0c          	mov    %eax,0xc(%esp)
  800e24:	89 54 24 10          	mov    %edx,0x10(%esp)
  800e28:	8b 45 10             	mov    0x10(%ebp),%eax
  800e2b:	89 44 24 08          	mov    %eax,0x8(%esp)
  800e2f:	8b 45 0c             	mov    0xc(%ebp),%eax
  800e32:	89 44 24 04          	mov    %eax,0x4(%esp)
  800e36:	8b 45 08             	mov    0x8(%ebp),%eax
  800e39:	89 04 24             	mov    %eax,(%esp)
  800e3c:	e8 31 ff ff ff       	call   800d72 <printnum>
  800e41:	eb 23                	jmp    800e66 <printnum+0xf4>
    } else {
        // print any needed pad characters before first digit
        while (-- width > 0)
            putch(padc, putdat, fd);
  800e43:	8b 45 0c             	mov    0xc(%ebp),%eax
  800e46:	89 44 24 08          	mov    %eax,0x8(%esp)
  800e4a:	8b 45 10             	mov    0x10(%ebp),%eax
  800e4d:	89 44 24 04          	mov    %eax,0x4(%esp)
  800e51:	8b 45 24             	mov    0x24(%ebp),%eax
  800e54:	89 04 24             	mov    %eax,(%esp)
  800e57:	8b 45 08             	mov    0x8(%ebp),%eax
  800e5a:	ff d0                	call   *%eax
    // first recursively print all preceding (more significant) digits
    if (num >= base) {
        printnum(putch, fd, putdat, result, base, width - 1, padc);
    } else {
        // print any needed pad characters before first digit
        while (-- width > 0)
  800e5c:	83 6d 20 01          	subl   $0x1,0x20(%ebp)
  800e60:	83 7d 20 00          	cmpl   $0x0,0x20(%ebp)
  800e64:	7f dd                	jg     800e43 <printnum+0xd1>
            putch(padc, putdat, fd);
    }
    // then print this (the least significant) digit
    putch("0123456789abcdef"[mod], putdat, fd);
  800e66:	8b 45 d8             	mov    -0x28(%ebp),%eax
  800e69:	05 64 1d 80 00       	add    $0x801d64,%eax
  800e6e:	0f b6 00             	movzbl (%eax),%eax
  800e71:	0f be c0             	movsbl %al,%eax
  800e74:	8b 55 0c             	mov    0xc(%ebp),%edx
  800e77:	89 54 24 08          	mov    %edx,0x8(%esp)
  800e7b:	8b 55 10             	mov    0x10(%ebp),%edx
  800e7e:	89 54 24 04          	mov    %edx,0x4(%esp)
  800e82:	89 04 24             	mov    %eax,(%esp)
  800e85:	8b 45 08             	mov    0x8(%ebp),%eax
  800e88:	ff d0                	call   *%eax
}
  800e8a:	c9                   	leave  
  800e8b:	c3                   	ret    

00800e8c <getuint>:
 * getuint - get an unsigned int of various possible sizes from a varargs list
 * @ap:         a varargs list pointer
 * @lflag:      determines the size of the vararg that @ap points to
 * */
static unsigned long long
getuint(va_list *ap, int lflag) {
  800e8c:	55                   	push   %ebp
  800e8d:	89 e5                	mov    %esp,%ebp
    if (lflag >= 2) {
  800e8f:	83 7d 0c 01          	cmpl   $0x1,0xc(%ebp)
  800e93:	7e 14                	jle    800ea9 <getuint+0x1d>
        return va_arg(*ap, unsigned long long);
  800e95:	8b 45 08             	mov    0x8(%ebp),%eax
  800e98:	8b 00                	mov    (%eax),%eax
  800e9a:	8d 48 08             	lea    0x8(%eax),%ecx
  800e9d:	8b 55 08             	mov    0x8(%ebp),%edx
  800ea0:	89 0a                	mov    %ecx,(%edx)
  800ea2:	8b 50 04             	mov    0x4(%eax),%edx
  800ea5:	8b 00                	mov    (%eax),%eax
  800ea7:	eb 30                	jmp    800ed9 <getuint+0x4d>
    }
    else if (lflag) {
  800ea9:	83 7d 0c 00          	cmpl   $0x0,0xc(%ebp)
  800ead:	74 16                	je     800ec5 <getuint+0x39>
        return va_arg(*ap, unsigned long);
  800eaf:	8b 45 08             	mov    0x8(%ebp),%eax
  800eb2:	8b 00                	mov    (%eax),%eax
  800eb4:	8d 48 04             	lea    0x4(%eax),%ecx
  800eb7:	8b 55 08             	mov    0x8(%ebp),%edx
  800eba:	89 0a                	mov    %ecx,(%edx)
  800ebc:	8b 00                	mov    (%eax),%eax
  800ebe:	ba 00 00 00 00       	mov    $0x0,%edx
  800ec3:	eb 14                	jmp    800ed9 <getuint+0x4d>
    }
    else {
        return va_arg(*ap, unsigned int);
  800ec5:	8b 45 08             	mov    0x8(%ebp),%eax
  800ec8:	8b 00                	mov    (%eax),%eax
  800eca:	8d 48 04             	lea    0x4(%eax),%ecx
  800ecd:	8b 55 08             	mov    0x8(%ebp),%edx
  800ed0:	89 0a                	mov    %ecx,(%edx)
  800ed2:	8b 00                	mov    (%eax),%eax
  800ed4:	ba 00 00 00 00       	mov    $0x0,%edx
    }
}
  800ed9:	5d                   	pop    %ebp
  800eda:	c3                   	ret    

00800edb <getint>:
 * getint - same as getuint but signed, we can't use getuint because of sign extension
 * @ap:         a varargs list pointer
 * @lflag:      determines the size of the vararg that @ap points to
 * */
static long long
getint(va_list *ap, int lflag) {
  800edb:	55                   	push   %ebp
  800edc:	89 e5                	mov    %esp,%ebp
    if (lflag >= 2) {
  800ede:	83 7d 0c 01          	cmpl   $0x1,0xc(%ebp)
  800ee2:	7e 14                	jle    800ef8 <getint+0x1d>
        return va_arg(*ap, long long);
  800ee4:	8b 45 08             	mov    0x8(%ebp),%eax
  800ee7:	8b 00                	mov    (%eax),%eax
  800ee9:	8d 48 08             	lea    0x8(%eax),%ecx
  800eec:	8b 55 08             	mov    0x8(%ebp),%edx
  800eef:	89 0a                	mov    %ecx,(%edx)
  800ef1:	8b 50 04             	mov    0x4(%eax),%edx
  800ef4:	8b 00                	mov    (%eax),%eax
  800ef6:	eb 28                	jmp    800f20 <getint+0x45>
    }
    else if (lflag) {
  800ef8:	83 7d 0c 00          	cmpl   $0x0,0xc(%ebp)
  800efc:	74 12                	je     800f10 <getint+0x35>
        return va_arg(*ap, long);
  800efe:	8b 45 08             	mov    0x8(%ebp),%eax
  800f01:	8b 00                	mov    (%eax),%eax
  800f03:	8d 48 04             	lea    0x4(%eax),%ecx
  800f06:	8b 55 08             	mov    0x8(%ebp),%edx
  800f09:	89 0a                	mov    %ecx,(%edx)
  800f0b:	8b 00                	mov    (%eax),%eax
  800f0d:	99                   	cltd   
  800f0e:	eb 10                	jmp    800f20 <getint+0x45>
    }
    else {
        return va_arg(*ap, int);
  800f10:	8b 45 08             	mov    0x8(%ebp),%eax
  800f13:	8b 00                	mov    (%eax),%eax
  800f15:	8d 48 04             	lea    0x4(%eax),%ecx
  800f18:	8b 55 08             	mov    0x8(%ebp),%edx
  800f1b:	89 0a                	mov    %ecx,(%edx)
  800f1d:	8b 00                	mov    (%eax),%eax
  800f1f:	99                   	cltd   
    }
}
  800f20:	5d                   	pop    %ebp
  800f21:	c3                   	ret    

00800f22 <printfmt>:
 * @fd:         file descriptor
 * @putdat:     used by @putch function
 * @fmt:        the format string to use
 * */
void
printfmt(void (*putch)(int, void*, int), int fd, void *putdat, const char *fmt, ...) {
  800f22:	55                   	push   %ebp
  800f23:	89 e5                	mov    %esp,%ebp
  800f25:	83 ec 38             	sub    $0x38,%esp
    va_list ap;

    va_start(ap, fmt);
  800f28:	8d 45 18             	lea    0x18(%ebp),%eax
  800f2b:	89 45 f4             	mov    %eax,-0xc(%ebp)
    vprintfmt(putch, fd, putdat, fmt, ap);
  800f2e:	8b 45 f4             	mov    -0xc(%ebp),%eax
  800f31:	89 44 24 10          	mov    %eax,0x10(%esp)
  800f35:	8b 45 14             	mov    0x14(%ebp),%eax
  800f38:	89 44 24 0c          	mov    %eax,0xc(%esp)
  800f3c:	8b 45 10             	mov    0x10(%ebp),%eax
  800f3f:	89 44 24 08          	mov    %eax,0x8(%esp)
  800f43:	8b 45 0c             	mov    0xc(%ebp),%eax
  800f46:	89 44 24 04          	mov    %eax,0x4(%esp)
  800f4a:	8b 45 08             	mov    0x8(%ebp),%eax
  800f4d:	89 04 24             	mov    %eax,(%esp)
  800f50:	e8 02 00 00 00       	call   800f57 <vprintfmt>
    va_end(ap);
}
  800f55:	c9                   	leave  
  800f56:	c3                   	ret    

00800f57 <vprintfmt>:
 *
 * Call this function if you are already dealing with a va_list.
 * Or you probably want printfmt() instead.
 * */
void
vprintfmt(void (*putch)(int, void*, int), int fd, void *putdat, const char *fmt, va_list ap) {
  800f57:	55                   	push   %ebp
  800f58:	89 e5                	mov    %esp,%ebp
  800f5a:	56                   	push   %esi
  800f5b:	53                   	push   %ebx
  800f5c:	83 ec 40             	sub    $0x40,%esp
    register int ch, err;
    unsigned long long num;
    int base, width, precision, lflag, altflag;

    while (1) {
        while ((ch = *(unsigned char *)fmt ++) != '%') {
  800f5f:	eb 1f                	jmp    800f80 <vprintfmt+0x29>
            if (ch == '\0') {
  800f61:	85 db                	test   %ebx,%ebx
  800f63:	75 05                	jne    800f6a <vprintfmt+0x13>
                return;
  800f65:	e9 33 04 00 00       	jmp    80139d <vprintfmt+0x446>
            }
            putch(ch, putdat, fd);
  800f6a:	8b 45 0c             	mov    0xc(%ebp),%eax
  800f6d:	89 44 24 08          	mov    %eax,0x8(%esp)
  800f71:	8b 45 10             	mov    0x10(%ebp),%eax
  800f74:	89 44 24 04          	mov    %eax,0x4(%esp)
  800f78:	89 1c 24             	mov    %ebx,(%esp)
  800f7b:	8b 45 08             	mov    0x8(%ebp),%eax
  800f7e:	ff d0                	call   *%eax
    register int ch, err;
    unsigned long long num;
    int base, width, precision, lflag, altflag;

    while (1) {
        while ((ch = *(unsigned char *)fmt ++) != '%') {
  800f80:	8b 45 14             	mov    0x14(%ebp),%eax
  800f83:	8d 50 01             	lea    0x1(%eax),%edx
  800f86:	89 55 14             	mov    %edx,0x14(%ebp)
  800f89:	0f b6 00             	movzbl (%eax),%eax
  800f8c:	0f b6 d8             	movzbl %al,%ebx
  800f8f:	83 fb 25             	cmp    $0x25,%ebx
  800f92:	75 cd                	jne    800f61 <vprintfmt+0xa>
            }
            putch(ch, putdat, fd);
        }

        // Process a %-escape sequence
        char padc = ' ';
  800f94:	c6 45 db 20          	movb   $0x20,-0x25(%ebp)
        width = precision = -1;
  800f98:	c7 45 e4 ff ff ff ff 	movl   $0xffffffff,-0x1c(%ebp)
  800f9f:	8b 45 e4             	mov    -0x1c(%ebp),%eax
  800fa2:	89 45 e8             	mov    %eax,-0x18(%ebp)
        lflag = altflag = 0;
  800fa5:	c7 45 dc 00 00 00 00 	movl   $0x0,-0x24(%ebp)
  800fac:	8b 45 dc             	mov    -0x24(%ebp),%eax
  800faf:	89 45 e0             	mov    %eax,-0x20(%ebp)

    reswitch:
        switch (ch = *(unsigned char *)fmt ++) {
  800fb2:	8b 45 14             	mov    0x14(%ebp),%eax
  800fb5:	8d 50 01             	lea    0x1(%eax),%edx
  800fb8:	89 55 14             	mov    %edx,0x14(%ebp)
  800fbb:	0f b6 00             	movzbl (%eax),%eax
  800fbe:	0f b6 d8             	movzbl %al,%ebx
  800fc1:	8d 43 dd             	lea    -0x23(%ebx),%eax
  800fc4:	83 f8 55             	cmp    $0x55,%eax
  800fc7:	0f 87 98 03 00 00    	ja     801365 <vprintfmt+0x40e>
  800fcd:	8b 04 85 88 1d 80 00 	mov    0x801d88(,%eax,4),%eax
  800fd4:	ff e0                	jmp    *%eax

        // flag to pad on the right
        case '-':
            padc = '-';
  800fd6:	c6 45 db 2d          	movb   $0x2d,-0x25(%ebp)
            goto reswitch;
  800fda:	eb d6                	jmp    800fb2 <vprintfmt+0x5b>

        // flag to pad with 0's instead of spaces
        case '0':
            padc = '0';
  800fdc:	c6 45 db 30          	movb   $0x30,-0x25(%ebp)
            goto reswitch;
  800fe0:	eb d0                	jmp    800fb2 <vprintfmt+0x5b>

        // width field
        case '1' ... '9':
            for (precision = 0; ; ++ fmt) {
  800fe2:	c7 45 e4 00 00 00 00 	movl   $0x0,-0x1c(%ebp)
                precision = precision * 10 + ch - '0';
  800fe9:	8b 55 e4             	mov    -0x1c(%ebp),%edx
  800fec:	89 d0                	mov    %edx,%eax
  800fee:	c1 e0 02             	shl    $0x2,%eax
  800ff1:	01 d0                	add    %edx,%eax
  800ff3:	01 c0                	add    %eax,%eax
  800ff5:	01 d8                	add    %ebx,%eax
  800ff7:	83 e8 30             	sub    $0x30,%eax
  800ffa:	89 45 e4             	mov    %eax,-0x1c(%ebp)
                ch = *fmt;
  800ffd:	8b 45 14             	mov    0x14(%ebp),%eax
  801000:	0f b6 00             	movzbl (%eax),%eax
  801003:	0f be d8             	movsbl %al,%ebx
                if (ch < '0' || ch > '9') {
  801006:	83 fb 2f             	cmp    $0x2f,%ebx
  801009:	7e 0b                	jle    801016 <vprintfmt+0xbf>
  80100b:	83 fb 39             	cmp    $0x39,%ebx
  80100e:	7f 06                	jg     801016 <vprintfmt+0xbf>
            padc = '0';
            goto reswitch;

        // width field
        case '1' ... '9':
            for (precision = 0; ; ++ fmt) {
  801010:	83 45 14 01          	addl   $0x1,0x14(%ebp)
                precision = precision * 10 + ch - '0';
                ch = *fmt;
                if (ch < '0' || ch > '9') {
                    break;
                }
            }
  801014:	eb d3                	jmp    800fe9 <vprintfmt+0x92>
            goto process_precision;
  801016:	eb 33                	jmp    80104b <vprintfmt+0xf4>

        case '*':
            precision = va_arg(ap, int);
  801018:	8b 45 18             	mov    0x18(%ebp),%eax
  80101b:	8d 50 04             	lea    0x4(%eax),%edx
  80101e:	89 55 18             	mov    %edx,0x18(%ebp)
  801021:	8b 00                	mov    (%eax),%eax
  801023:	89 45 e4             	mov    %eax,-0x1c(%ebp)
            goto process_precision;
  801026:	eb 23                	jmp    80104b <vprintfmt+0xf4>

        case '.':
            if (width < 0)
  801028:	83 7d e8 00          	cmpl   $0x0,-0x18(%ebp)
  80102c:	79 0c                	jns    80103a <vprintfmt+0xe3>
                width = 0;
  80102e:	c7 45 e8 00 00 00 00 	movl   $0x0,-0x18(%ebp)
            goto reswitch;
  801035:	e9 78 ff ff ff       	jmp    800fb2 <vprintfmt+0x5b>
  80103a:	e9 73 ff ff ff       	jmp    800fb2 <vprintfmt+0x5b>

        case '#':
            altflag = 1;
  80103f:	c7 45 dc 01 00 00 00 	movl   $0x1,-0x24(%ebp)
            goto reswitch;
  801046:	e9 67 ff ff ff       	jmp    800fb2 <vprintfmt+0x5b>

        process_precision:
            if (width < 0)
  80104b:	83 7d e8 00          	cmpl   $0x0,-0x18(%ebp)
  80104f:	79 12                	jns    801063 <vprintfmt+0x10c>
                width = precision, precision = -1;
  801051:	8b 45 e4             	mov    -0x1c(%ebp),%eax
  801054:	89 45 e8             	mov    %eax,-0x18(%ebp)
  801057:	c7 45 e4 ff ff ff ff 	movl   $0xffffffff,-0x1c(%ebp)
            goto reswitch;
  80105e:	e9 4f ff ff ff       	jmp    800fb2 <vprintfmt+0x5b>
  801063:	e9 4a ff ff ff       	jmp    800fb2 <vprintfmt+0x5b>

        // long flag (doubled for long long)
        case 'l':
            lflag ++;
  801068:	83 45 e0 01          	addl   $0x1,-0x20(%ebp)
            goto reswitch;
  80106c:	e9 41 ff ff ff       	jmp    800fb2 <vprintfmt+0x5b>

        // character
        case 'c':
            putch(va_arg(ap, int), putdat, fd);
  801071:	8b 45 18             	mov    0x18(%ebp),%eax
  801074:	8d 50 04             	lea    0x4(%eax),%edx
  801077:	89 55 18             	mov    %edx,0x18(%ebp)
  80107a:	8b 00                	mov    (%eax),%eax
  80107c:	8b 55 0c             	mov    0xc(%ebp),%edx
  80107f:	89 54 24 08          	mov    %edx,0x8(%esp)
  801083:	8b 55 10             	mov    0x10(%ebp),%edx
  801086:	89 54 24 04          	mov    %edx,0x4(%esp)
  80108a:	89 04 24             	mov    %eax,(%esp)
  80108d:	8b 45 08             	mov    0x8(%ebp),%eax
  801090:	ff d0                	call   *%eax
            break;
  801092:	e9 00 03 00 00       	jmp    801397 <vprintfmt+0x440>

        // error message
        case 'e':
            err = va_arg(ap, int);
  801097:	8b 45 18             	mov    0x18(%ebp),%eax
  80109a:	8d 50 04             	lea    0x4(%eax),%edx
  80109d:	89 55 18             	mov    %edx,0x18(%ebp)
  8010a0:	8b 18                	mov    (%eax),%ebx
            if (err < 0) {
  8010a2:	85 db                	test   %ebx,%ebx
  8010a4:	79 02                	jns    8010a8 <vprintfmt+0x151>
                err = -err;
  8010a6:	f7 db                	neg    %ebx
            }
            if (err > MAXERROR || (p = error_string[err]) == NULL) {
  8010a8:	83 fb 18             	cmp    $0x18,%ebx
  8010ab:	7f 0b                	jg     8010b8 <vprintfmt+0x161>
  8010ad:	8b 34 9d 00 1d 80 00 	mov    0x801d00(,%ebx,4),%esi
  8010b4:	85 f6                	test   %esi,%esi
  8010b6:	75 2a                	jne    8010e2 <vprintfmt+0x18b>
                printfmt(putch, fd, putdat, "error %d", err);
  8010b8:	89 5c 24 10          	mov    %ebx,0x10(%esp)
  8010bc:	c7 44 24 0c 75 1d 80 	movl   $0x801d75,0xc(%esp)
  8010c3:	00 
  8010c4:	8b 45 10             	mov    0x10(%ebp),%eax
  8010c7:	89 44 24 08          	mov    %eax,0x8(%esp)
  8010cb:	8b 45 0c             	mov    0xc(%ebp),%eax
  8010ce:	89 44 24 04          	mov    %eax,0x4(%esp)
  8010d2:	8b 45 08             	mov    0x8(%ebp),%eax
  8010d5:	89 04 24             	mov    %eax,(%esp)
  8010d8:	e8 45 fe ff ff       	call   800f22 <printfmt>
            }
            else {
                printfmt(putch, fd, putdat, "%s", p);
            }
            break;
  8010dd:	e9 b5 02 00 00       	jmp    801397 <vprintfmt+0x440>
            }
            if (err > MAXERROR || (p = error_string[err]) == NULL) {
                printfmt(putch, fd, putdat, "error %d", err);
            }
            else {
                printfmt(putch, fd, putdat, "%s", p);
  8010e2:	89 74 24 10          	mov    %esi,0x10(%esp)
  8010e6:	c7 44 24 0c 7e 1d 80 	movl   $0x801d7e,0xc(%esp)
  8010ed:	00 
  8010ee:	8b 45 10             	mov    0x10(%ebp),%eax
  8010f1:	89 44 24 08          	mov    %eax,0x8(%esp)
  8010f5:	8b 45 0c             	mov    0xc(%ebp),%eax
  8010f8:	89 44 24 04          	mov    %eax,0x4(%esp)
  8010fc:	8b 45 08             	mov    0x8(%ebp),%eax
  8010ff:	89 04 24             	mov    %eax,(%esp)
  801102:	e8 1b fe ff ff       	call   800f22 <printfmt>
            }
            break;
  801107:	e9 8b 02 00 00       	jmp    801397 <vprintfmt+0x440>

        // string
        case 's':
            if ((p = va_arg(ap, char *)) == NULL) {
  80110c:	8b 45 18             	mov    0x18(%ebp),%eax
  80110f:	8d 50 04             	lea    0x4(%eax),%edx
  801112:	89 55 18             	mov    %edx,0x18(%ebp)
  801115:	8b 30                	mov    (%eax),%esi
  801117:	85 f6                	test   %esi,%esi
  801119:	75 05                	jne    801120 <vprintfmt+0x1c9>
                p = "(null)";
  80111b:	be 81 1d 80 00       	mov    $0x801d81,%esi
            }
            if (width > 0 && padc != '-') {
  801120:	83 7d e8 00          	cmpl   $0x0,-0x18(%ebp)
  801124:	7e 45                	jle    80116b <vprintfmt+0x214>
  801126:	80 7d db 2d          	cmpb   $0x2d,-0x25(%ebp)
  80112a:	74 3f                	je     80116b <vprintfmt+0x214>
                for (width -= strnlen(p, precision); width > 0; width --) {
  80112c:	8b 5d e8             	mov    -0x18(%ebp),%ebx
  80112f:	8b 45 e4             	mov    -0x1c(%ebp),%eax
  801132:	89 44 24 04          	mov    %eax,0x4(%esp)
  801136:	89 34 24             	mov    %esi,(%esp)
  801139:	e8 3b 04 00 00       	call   801579 <strnlen>
  80113e:	29 c3                	sub    %eax,%ebx
  801140:	89 d8                	mov    %ebx,%eax
  801142:	89 45 e8             	mov    %eax,-0x18(%ebp)
  801145:	eb 1e                	jmp    801165 <vprintfmt+0x20e>
                    putch(padc, putdat, fd);
  801147:	0f be 45 db          	movsbl -0x25(%ebp),%eax
  80114b:	8b 55 0c             	mov    0xc(%ebp),%edx
  80114e:	89 54 24 08          	mov    %edx,0x8(%esp)
  801152:	8b 55 10             	mov    0x10(%ebp),%edx
  801155:	89 54 24 04          	mov    %edx,0x4(%esp)
  801159:	89 04 24             	mov    %eax,(%esp)
  80115c:	8b 45 08             	mov    0x8(%ebp),%eax
  80115f:	ff d0                	call   *%eax
        case 's':
            if ((p = va_arg(ap, char *)) == NULL) {
                p = "(null)";
            }
            if (width > 0 && padc != '-') {
                for (width -= strnlen(p, precision); width > 0; width --) {
  801161:	83 6d e8 01          	subl   $0x1,-0x18(%ebp)
  801165:	83 7d e8 00          	cmpl   $0x0,-0x18(%ebp)
  801169:	7f dc                	jg     801147 <vprintfmt+0x1f0>
                    putch(padc, putdat, fd);
                }
            }
            for (; (ch = *p ++) != '\0' && (precision < 0 || -- precision >= 0); width --) {
  80116b:	eb 46                	jmp    8011b3 <vprintfmt+0x25c>
                if (altflag && (ch < ' ' || ch > '~')) {
  80116d:	83 7d dc 00          	cmpl   $0x0,-0x24(%ebp)
  801171:	74 26                	je     801199 <vprintfmt+0x242>
  801173:	83 fb 1f             	cmp    $0x1f,%ebx
  801176:	7e 05                	jle    80117d <vprintfmt+0x226>
  801178:	83 fb 7e             	cmp    $0x7e,%ebx
  80117b:	7e 1c                	jle    801199 <vprintfmt+0x242>
                    putch('?', putdat, fd);
  80117d:	8b 45 0c             	mov    0xc(%ebp),%eax
  801180:	89 44 24 08          	mov    %eax,0x8(%esp)
  801184:	8b 45 10             	mov    0x10(%ebp),%eax
  801187:	89 44 24 04          	mov    %eax,0x4(%esp)
  80118b:	c7 04 24 3f 00 00 00 	movl   $0x3f,(%esp)
  801192:	8b 45 08             	mov    0x8(%ebp),%eax
  801195:	ff d0                	call   *%eax
  801197:	eb 16                	jmp    8011af <vprintfmt+0x258>
                }
                else {
                    putch(ch, putdat, fd);
  801199:	8b 45 0c             	mov    0xc(%ebp),%eax
  80119c:	89 44 24 08          	mov    %eax,0x8(%esp)
  8011a0:	8b 45 10             	mov    0x10(%ebp),%eax
  8011a3:	89 44 24 04          	mov    %eax,0x4(%esp)
  8011a7:	89 1c 24             	mov    %ebx,(%esp)
  8011aa:	8b 45 08             	mov    0x8(%ebp),%eax
  8011ad:	ff d0                	call   *%eax
            if (width > 0 && padc != '-') {
                for (width -= strnlen(p, precision); width > 0; width --) {
                    putch(padc, putdat, fd);
                }
            }
            for (; (ch = *p ++) != '\0' && (precision < 0 || -- precision >= 0); width --) {
  8011af:	83 6d e8 01          	subl   $0x1,-0x18(%ebp)
  8011b3:	89 f0                	mov    %esi,%eax
  8011b5:	8d 70 01             	lea    0x1(%eax),%esi
  8011b8:	0f b6 00             	movzbl (%eax),%eax
  8011bb:	0f be d8             	movsbl %al,%ebx
  8011be:	85 db                	test   %ebx,%ebx
  8011c0:	74 10                	je     8011d2 <vprintfmt+0x27b>
  8011c2:	83 7d e4 00          	cmpl   $0x0,-0x1c(%ebp)
  8011c6:	78 a5                	js     80116d <vprintfmt+0x216>
  8011c8:	83 6d e4 01          	subl   $0x1,-0x1c(%ebp)
  8011cc:	83 7d e4 00          	cmpl   $0x0,-0x1c(%ebp)
  8011d0:	79 9b                	jns    80116d <vprintfmt+0x216>
                }
                else {
                    putch(ch, putdat, fd);
                }
            }
            for (; width > 0; width --) {
  8011d2:	eb 1e                	jmp    8011f2 <vprintfmt+0x29b>
                putch(' ', putdat, fd);
  8011d4:	8b 45 0c             	mov    0xc(%ebp),%eax
  8011d7:	89 44 24 08          	mov    %eax,0x8(%esp)
  8011db:	8b 45 10             	mov    0x10(%ebp),%eax
  8011de:	89 44 24 04          	mov    %eax,0x4(%esp)
  8011e2:	c7 04 24 20 00 00 00 	movl   $0x20,(%esp)
  8011e9:	8b 45 08             	mov    0x8(%ebp),%eax
  8011ec:	ff d0                	call   *%eax
                }
                else {
                    putch(ch, putdat, fd);
                }
            }
            for (; width > 0; width --) {
  8011ee:	83 6d e8 01          	subl   $0x1,-0x18(%ebp)
  8011f2:	83 7d e8 00          	cmpl   $0x0,-0x18(%ebp)
  8011f6:	7f dc                	jg     8011d4 <vprintfmt+0x27d>
                putch(' ', putdat, fd);
            }
            break;
  8011f8:	e9 9a 01 00 00       	jmp    801397 <vprintfmt+0x440>

        // (signed) decimal
        case 'd':
            num = getint(&ap, lflag);
  8011fd:	8b 45 e0             	mov    -0x20(%ebp),%eax
  801200:	89 44 24 04          	mov    %eax,0x4(%esp)
  801204:	8d 45 18             	lea    0x18(%ebp),%eax
  801207:	89 04 24             	mov    %eax,(%esp)
  80120a:	e8 cc fc ff ff       	call   800edb <getint>
  80120f:	89 45 f0             	mov    %eax,-0x10(%ebp)
  801212:	89 55 f4             	mov    %edx,-0xc(%ebp)
            if ((long long)num < 0) {
  801215:	8b 45 f0             	mov    -0x10(%ebp),%eax
  801218:	8b 55 f4             	mov    -0xc(%ebp),%edx
  80121b:	85 d2                	test   %edx,%edx
  80121d:	79 2d                	jns    80124c <vprintfmt+0x2f5>
                putch('-', putdat, fd);
  80121f:	8b 45 0c             	mov    0xc(%ebp),%eax
  801222:	89 44 24 08          	mov    %eax,0x8(%esp)
  801226:	8b 45 10             	mov    0x10(%ebp),%eax
  801229:	89 44 24 04          	mov    %eax,0x4(%esp)
  80122d:	c7 04 24 2d 00 00 00 	movl   $0x2d,(%esp)
  801234:	8b 45 08             	mov    0x8(%ebp),%eax
  801237:	ff d0                	call   *%eax
                num = -(long long)num;
  801239:	8b 45 f0             	mov    -0x10(%ebp),%eax
  80123c:	8b 55 f4             	mov    -0xc(%ebp),%edx
  80123f:	f7 d8                	neg    %eax
  801241:	83 d2 00             	adc    $0x0,%edx
  801244:	f7 da                	neg    %edx
  801246:	89 45 f0             	mov    %eax,-0x10(%ebp)
  801249:	89 55 f4             	mov    %edx,-0xc(%ebp)
            }
            base = 10;
  80124c:	c7 45 ec 0a 00 00 00 	movl   $0xa,-0x14(%ebp)
            goto number;
  801253:	e9 b6 00 00 00       	jmp    80130e <vprintfmt+0x3b7>

        // unsigned decimal
        case 'u':
            num = getuint(&ap, lflag);
  801258:	8b 45 e0             	mov    -0x20(%ebp),%eax
  80125b:	89 44 24 04          	mov    %eax,0x4(%esp)
  80125f:	8d 45 18             	lea    0x18(%ebp),%eax
  801262:	89 04 24             	mov    %eax,(%esp)
  801265:	e8 22 fc ff ff       	call   800e8c <getuint>
  80126a:	89 45 f0             	mov    %eax,-0x10(%ebp)
  80126d:	89 55 f4             	mov    %edx,-0xc(%ebp)
            base = 10;
  801270:	c7 45 ec 0a 00 00 00 	movl   $0xa,-0x14(%ebp)
            goto number;
  801277:	e9 92 00 00 00       	jmp    80130e <vprintfmt+0x3b7>

        // (unsigned) octal
        case 'o':
            num = getuint(&ap, lflag);
  80127c:	8b 45 e0             	mov    -0x20(%ebp),%eax
  80127f:	89 44 24 04          	mov    %eax,0x4(%esp)
  801283:	8d 45 18             	lea    0x18(%ebp),%eax
  801286:	89 04 24             	mov    %eax,(%esp)
  801289:	e8 fe fb ff ff       	call   800e8c <getuint>
  80128e:	89 45 f0             	mov    %eax,-0x10(%ebp)
  801291:	89 55 f4             	mov    %edx,-0xc(%ebp)
            base = 8;
  801294:	c7 45 ec 08 00 00 00 	movl   $0x8,-0x14(%ebp)
            goto number;
  80129b:	eb 71                	jmp    80130e <vprintfmt+0x3b7>

        // pointer
        case 'p':
            putch('0', putdat, fd);
  80129d:	8b 45 0c             	mov    0xc(%ebp),%eax
  8012a0:	89 44 24 08          	mov    %eax,0x8(%esp)
  8012a4:	8b 45 10             	mov    0x10(%ebp),%eax
  8012a7:	89 44 24 04          	mov    %eax,0x4(%esp)
  8012ab:	c7 04 24 30 00 00 00 	movl   $0x30,(%esp)
  8012b2:	8b 45 08             	mov    0x8(%ebp),%eax
  8012b5:	ff d0                	call   *%eax
            putch('x', putdat, fd);
  8012b7:	8b 45 0c             	mov    0xc(%ebp),%eax
  8012ba:	89 44 24 08          	mov    %eax,0x8(%esp)
  8012be:	8b 45 10             	mov    0x10(%ebp),%eax
  8012c1:	89 44 24 04          	mov    %eax,0x4(%esp)
  8012c5:	c7 04 24 78 00 00 00 	movl   $0x78,(%esp)
  8012cc:	8b 45 08             	mov    0x8(%ebp),%eax
  8012cf:	ff d0                	call   *%eax
            num = (unsigned long long)(uintptr_t)va_arg(ap, void *);
  8012d1:	8b 45 18             	mov    0x18(%ebp),%eax
  8012d4:	8d 50 04             	lea    0x4(%eax),%edx
  8012d7:	89 55 18             	mov    %edx,0x18(%ebp)
  8012da:	8b 00                	mov    (%eax),%eax
  8012dc:	89 45 f0             	mov    %eax,-0x10(%ebp)
  8012df:	c7 45 f4 00 00 00 00 	movl   $0x0,-0xc(%ebp)
            base = 16;
  8012e6:	c7 45 ec 10 00 00 00 	movl   $0x10,-0x14(%ebp)
            goto number;
  8012ed:	eb 1f                	jmp    80130e <vprintfmt+0x3b7>

        // (unsigned) hexadecimal
        case 'x':
            num = getuint(&ap, lflag);
  8012ef:	8b 45 e0             	mov    -0x20(%ebp),%eax
  8012f2:	89 44 24 04          	mov    %eax,0x4(%esp)
  8012f6:	8d 45 18             	lea    0x18(%ebp),%eax
  8012f9:	89 04 24             	mov    %eax,(%esp)
  8012fc:	e8 8b fb ff ff       	call   800e8c <getuint>
  801301:	89 45 f0             	mov    %eax,-0x10(%ebp)
  801304:	89 55 f4             	mov    %edx,-0xc(%ebp)
            base = 16;
  801307:	c7 45 ec 10 00 00 00 	movl   $0x10,-0x14(%ebp)
        number:
            printnum(putch, fd, putdat, num, base, width, padc);
  80130e:	0f be 55 db          	movsbl -0x25(%ebp),%edx
  801312:	8b 45 ec             	mov    -0x14(%ebp),%eax
  801315:	89 54 24 1c          	mov    %edx,0x1c(%esp)
  801319:	8b 55 e8             	mov    -0x18(%ebp),%edx
  80131c:	89 54 24 18          	mov    %edx,0x18(%esp)
  801320:	89 44 24 14          	mov    %eax,0x14(%esp)
  801324:	8b 45 f0             	mov    -0x10(%ebp),%eax
  801327:	8b 55 f4             	mov    -0xc(%ebp),%edx
  80132a:	89 44 24 0c          	mov    %eax,0xc(%esp)
  80132e:	89 54 24 10          	mov    %edx,0x10(%esp)
  801332:	8b 45 10             	mov    0x10(%ebp),%eax
  801335:	89 44 24 08          	mov    %eax,0x8(%esp)
  801339:	8b 45 0c             	mov    0xc(%ebp),%eax
  80133c:	89 44 24 04          	mov    %eax,0x4(%esp)
  801340:	8b 45 08             	mov    0x8(%ebp),%eax
  801343:	89 04 24             	mov    %eax,(%esp)
  801346:	e8 27 fa ff ff       	call   800d72 <printnum>
            break;
  80134b:	eb 4a                	jmp    801397 <vprintfmt+0x440>

        // escaped '%' character
        case '%':
            putch(ch, putdat, fd);
  80134d:	8b 45 0c             	mov    0xc(%ebp),%eax
  801350:	89 44 24 08          	mov    %eax,0x8(%esp)
  801354:	8b 45 10             	mov    0x10(%ebp),%eax
  801357:	89 44 24 04          	mov    %eax,0x4(%esp)
  80135b:	89 1c 24             	mov    %ebx,(%esp)
  80135e:	8b 45 08             	mov    0x8(%ebp),%eax
  801361:	ff d0                	call   *%eax
            break;
  801363:	eb 32                	jmp    801397 <vprintfmt+0x440>

        // unrecognized escape sequence - just print it literally
        default:
            putch('%', putdat, fd);
  801365:	8b 45 0c             	mov    0xc(%ebp),%eax
  801368:	89 44 24 08          	mov    %eax,0x8(%esp)
  80136c:	8b 45 10             	mov    0x10(%ebp),%eax
  80136f:	89 44 24 04          	mov    %eax,0x4(%esp)
  801373:	c7 04 24 25 00 00 00 	movl   $0x25,(%esp)
  80137a:	8b 45 08             	mov    0x8(%ebp),%eax
  80137d:	ff d0                	call   *%eax
            for (fmt --; fmt[-1] != '%'; fmt --)
  80137f:	83 6d 14 01          	subl   $0x1,0x14(%ebp)
  801383:	eb 04                	jmp    801389 <vprintfmt+0x432>
  801385:	83 6d 14 01          	subl   $0x1,0x14(%ebp)
  801389:	8b 45 14             	mov    0x14(%ebp),%eax
  80138c:	83 e8 01             	sub    $0x1,%eax
  80138f:	0f b6 00             	movzbl (%eax),%eax
  801392:	3c 25                	cmp    $0x25,%al
  801394:	75 ef                	jne    801385 <vprintfmt+0x42e>
                /* do nothing */;
            break;
  801396:	90                   	nop
        }
    }
  801397:	90                   	nop
    register int ch, err;
    unsigned long long num;
    int base, width, precision, lflag, altflag;

    while (1) {
        while ((ch = *(unsigned char *)fmt ++) != '%') {
  801398:	e9 e3 fb ff ff       	jmp    800f80 <vprintfmt+0x29>
            for (fmt --; fmt[-1] != '%'; fmt --)
                /* do nothing */;
            break;
        }
    }
}
  80139d:	83 c4 40             	add    $0x40,%esp
  8013a0:	5b                   	pop    %ebx
  8013a1:	5e                   	pop    %esi
  8013a2:	5d                   	pop    %ebp
  8013a3:	c3                   	ret    

008013a4 <sprintputch>:
 * sprintputch - 'print' a single character in a buffer
 * @ch:         the character will be printed
 * @b:          the buffer to place the character @ch
 * */
static void
sprintputch(int ch, struct sprintbuf *b) {
  8013a4:	55                   	push   %ebp
  8013a5:	89 e5                	mov    %esp,%ebp
    b->cnt ++;
  8013a7:	8b 45 0c             	mov    0xc(%ebp),%eax
  8013aa:	8b 40 08             	mov    0x8(%eax),%eax
  8013ad:	8d 50 01             	lea    0x1(%eax),%edx
  8013b0:	8b 45 0c             	mov    0xc(%ebp),%eax
  8013b3:	89 50 08             	mov    %edx,0x8(%eax)
    if (b->buf < b->ebuf) {
  8013b6:	8b 45 0c             	mov    0xc(%ebp),%eax
  8013b9:	8b 10                	mov    (%eax),%edx
  8013bb:	8b 45 0c             	mov    0xc(%ebp),%eax
  8013be:	8b 40 04             	mov    0x4(%eax),%eax
  8013c1:	39 c2                	cmp    %eax,%edx
  8013c3:	73 12                	jae    8013d7 <sprintputch+0x33>
        *b->buf ++ = ch;
  8013c5:	8b 45 0c             	mov    0xc(%ebp),%eax
  8013c8:	8b 00                	mov    (%eax),%eax
  8013ca:	8d 48 01             	lea    0x1(%eax),%ecx
  8013cd:	8b 55 0c             	mov    0xc(%ebp),%edx
  8013d0:	89 0a                	mov    %ecx,(%edx)
  8013d2:	8b 55 08             	mov    0x8(%ebp),%edx
  8013d5:	88 10                	mov    %dl,(%eax)
    }
}
  8013d7:	5d                   	pop    %ebp
  8013d8:	c3                   	ret    

008013d9 <snprintf>:
 * @str:        the buffer to place the result into
 * @size:       the size of buffer, including the trailing null space
 * @fmt:        the format string to use
 * */
int
snprintf(char *str, size_t size, const char *fmt, ...) {
  8013d9:	55                   	push   %ebp
  8013da:	89 e5                	mov    %esp,%ebp
  8013dc:	83 ec 28             	sub    $0x28,%esp
    va_list ap;
    int cnt;
    va_start(ap, fmt);
  8013df:	8d 45 14             	lea    0x14(%ebp),%eax
  8013e2:	89 45 f0             	mov    %eax,-0x10(%ebp)
    cnt = vsnprintf(str, size, fmt, ap);
  8013e5:	8b 45 f0             	mov    -0x10(%ebp),%eax
  8013e8:	89 44 24 0c          	mov    %eax,0xc(%esp)
  8013ec:	8b 45 10             	mov    0x10(%ebp),%eax
  8013ef:	89 44 24 08          	mov    %eax,0x8(%esp)
  8013f3:	8b 45 0c             	mov    0xc(%ebp),%eax
  8013f6:	89 44 24 04          	mov    %eax,0x4(%esp)
  8013fa:	8b 45 08             	mov    0x8(%ebp),%eax
  8013fd:	89 04 24             	mov    %eax,(%esp)
  801400:	e8 08 00 00 00       	call   80140d <vsnprintf>
  801405:	89 45 f4             	mov    %eax,-0xc(%ebp)
    va_end(ap);
    return cnt;
  801408:	8b 45 f4             	mov    -0xc(%ebp),%eax
}
  80140b:	c9                   	leave  
  80140c:	c3                   	ret    

0080140d <vsnprintf>:
 *
 * Call this function if you are already dealing with a va_list.
 * Or you probably want snprintf() instead.
 * */
int
vsnprintf(char *str, size_t size, const char *fmt, va_list ap) {
  80140d:	55                   	push   %ebp
  80140e:	89 e5                	mov    %esp,%ebp
  801410:	83 ec 38             	sub    $0x38,%esp
    struct sprintbuf b = {str, str + size - 1, 0};
  801413:	8b 45 08             	mov    0x8(%ebp),%eax
  801416:	89 45 ec             	mov    %eax,-0x14(%ebp)
  801419:	8b 45 0c             	mov    0xc(%ebp),%eax
  80141c:	8d 50 ff             	lea    -0x1(%eax),%edx
  80141f:	8b 45 08             	mov    0x8(%ebp),%eax
  801422:	01 d0                	add    %edx,%eax
  801424:	89 45 f0             	mov    %eax,-0x10(%ebp)
  801427:	c7 45 f4 00 00 00 00 	movl   $0x0,-0xc(%ebp)
    if (str == NULL || b.buf > b.ebuf) {
  80142e:	83 7d 08 00          	cmpl   $0x0,0x8(%ebp)
  801432:	74 0a                	je     80143e <vsnprintf+0x31>
  801434:	8b 55 ec             	mov    -0x14(%ebp),%edx
  801437:	8b 45 f0             	mov    -0x10(%ebp),%eax
  80143a:	39 c2                	cmp    %eax,%edx
  80143c:	76 07                	jbe    801445 <vsnprintf+0x38>
        return -E_INVAL;
  80143e:	b8 fd ff ff ff       	mov    $0xfffffffd,%eax
  801443:	eb 32                	jmp    801477 <vsnprintf+0x6a>
    }
    // print the string to the buffer
    vprintfmt((void*)sprintputch, NO_FD, &b, fmt, ap);
  801445:	8b 45 14             	mov    0x14(%ebp),%eax
  801448:	89 44 24 10          	mov    %eax,0x10(%esp)
  80144c:	8b 45 10             	mov    0x10(%ebp),%eax
  80144f:	89 44 24 0c          	mov    %eax,0xc(%esp)
  801453:	8d 45 ec             	lea    -0x14(%ebp),%eax
  801456:	89 44 24 08          	mov    %eax,0x8(%esp)
  80145a:	c7 44 24 04 d9 6a ff 	movl   $0xffff6ad9,0x4(%esp)
  801461:	ff 
  801462:	c7 04 24 a4 13 80 00 	movl   $0x8013a4,(%esp)
  801469:	e8 e9 fa ff ff       	call   800f57 <vprintfmt>
    // null terminate the buffer
    *b.buf = '\0';
  80146e:	8b 45 ec             	mov    -0x14(%ebp),%eax
  801471:	c6 00 00             	movb   $0x0,(%eax)
    return b.cnt;
  801474:	8b 45 f4             	mov    -0xc(%ebp),%eax
}
  801477:	c9                   	leave  
  801478:	c3                   	ret    

00801479 <rand>:
 * rand - returns a pseudo-random integer
 *
 * The rand() function return a value in the range [0, RAND_MAX].
 * */
int
rand(void) {
  801479:	55                   	push   %ebp
  80147a:	89 e5                	mov    %esp,%ebp
  80147c:	57                   	push   %edi
  80147d:	56                   	push   %esi
  80147e:	53                   	push   %ebx
  80147f:	83 ec 24             	sub    $0x24,%esp
    next = (next * 0x5DEECE66DLL + 0xBLL) & ((1LL << 48) - 1);
  801482:	a1 08 20 80 00       	mov    0x802008,%eax
  801487:	8b 15 0c 20 80 00    	mov    0x80200c,%edx
  80148d:	69 fa 6d e6 ec de    	imul   $0xdeece66d,%edx,%edi
  801493:	6b f0 05             	imul   $0x5,%eax,%esi
  801496:	01 f7                	add    %esi,%edi
  801498:	be 6d e6 ec de       	mov    $0xdeece66d,%esi
  80149d:	f7 e6                	mul    %esi
  80149f:	8d 34 17             	lea    (%edi,%edx,1),%esi
  8014a2:	89 f2                	mov    %esi,%edx
  8014a4:	83 c0 0b             	add    $0xb,%eax
  8014a7:	83 d2 00             	adc    $0x0,%edx
  8014aa:	89 c7                	mov    %eax,%edi
  8014ac:	83 e7 ff             	and    $0xffffffff,%edi
  8014af:	89 f9                	mov    %edi,%ecx
  8014b1:	0f b7 da             	movzwl %dx,%ebx
  8014b4:	89 0d 08 20 80 00    	mov    %ecx,0x802008
  8014ba:	89 1d 0c 20 80 00    	mov    %ebx,0x80200c
    unsigned long long result = (next >> 12);
  8014c0:	a1 08 20 80 00       	mov    0x802008,%eax
  8014c5:	8b 15 0c 20 80 00    	mov    0x80200c,%edx
  8014cb:	0f ac d0 0c          	shrd   $0xc,%edx,%eax
  8014cf:	c1 ea 0c             	shr    $0xc,%edx
  8014d2:	89 45 e0             	mov    %eax,-0x20(%ebp)
  8014d5:	89 55 e4             	mov    %edx,-0x1c(%ebp)
    return (int)do_div(result, RAND_MAX + 1);
  8014d8:	c7 45 dc 00 00 00 80 	movl   $0x80000000,-0x24(%ebp)
  8014df:	8b 45 e0             	mov    -0x20(%ebp),%eax
  8014e2:	8b 55 e4             	mov    -0x1c(%ebp),%edx
  8014e5:	89 45 d8             	mov    %eax,-0x28(%ebp)
  8014e8:	89 55 e8             	mov    %edx,-0x18(%ebp)
  8014eb:	8b 45 e8             	mov    -0x18(%ebp),%eax
  8014ee:	89 45 ec             	mov    %eax,-0x14(%ebp)
  8014f1:	83 7d e8 00          	cmpl   $0x0,-0x18(%ebp)
  8014f5:	74 1c                	je     801513 <rand+0x9a>
  8014f7:	8b 45 e8             	mov    -0x18(%ebp),%eax
  8014fa:	ba 00 00 00 00       	mov    $0x0,%edx
  8014ff:	f7 75 dc             	divl   -0x24(%ebp)
  801502:	89 55 ec             	mov    %edx,-0x14(%ebp)
  801505:	8b 45 e8             	mov    -0x18(%ebp),%eax
  801508:	ba 00 00 00 00       	mov    $0x0,%edx
  80150d:	f7 75 dc             	divl   -0x24(%ebp)
  801510:	89 45 e8             	mov    %eax,-0x18(%ebp)
  801513:	8b 45 d8             	mov    -0x28(%ebp),%eax
  801516:	8b 55 ec             	mov    -0x14(%ebp),%edx
  801519:	f7 75 dc             	divl   -0x24(%ebp)
  80151c:	89 45 d8             	mov    %eax,-0x28(%ebp)
  80151f:	89 55 d4             	mov    %edx,-0x2c(%ebp)
  801522:	8b 45 d8             	mov    -0x28(%ebp),%eax
  801525:	8b 55 e8             	mov    -0x18(%ebp),%edx
  801528:	89 45 e0             	mov    %eax,-0x20(%ebp)
  80152b:	89 55 e4             	mov    %edx,-0x1c(%ebp)
  80152e:	8b 45 d4             	mov    -0x2c(%ebp),%eax
}
  801531:	83 c4 24             	add    $0x24,%esp
  801534:	5b                   	pop    %ebx
  801535:	5e                   	pop    %esi
  801536:	5f                   	pop    %edi
  801537:	5d                   	pop    %ebp
  801538:	c3                   	ret    

00801539 <srand>:
/* *
 * srand - seed the random number generator with the given number
 * @seed:   the required seed number
 * */
void
srand(unsigned int seed) {
  801539:	55                   	push   %ebp
  80153a:	89 e5                	mov    %esp,%ebp
    next = seed;
  80153c:	8b 45 08             	mov    0x8(%ebp),%eax
  80153f:	ba 00 00 00 00       	mov    $0x0,%edx
  801544:	a3 08 20 80 00       	mov    %eax,0x802008
  801549:	89 15 0c 20 80 00    	mov    %edx,0x80200c
}
  80154f:	5d                   	pop    %ebp
  801550:	c3                   	ret    

00801551 <strlen>:
 * @s:      the input string
 *
 * The strlen() function returns the length of string @s.
 * */
size_t
strlen(const char *s) {
  801551:	55                   	push   %ebp
  801552:	89 e5                	mov    %esp,%ebp
  801554:	83 ec 10             	sub    $0x10,%esp
    size_t cnt = 0;
  801557:	c7 45 fc 00 00 00 00 	movl   $0x0,-0x4(%ebp)
    while (*s ++ != '\0') {
  80155e:	eb 04                	jmp    801564 <strlen+0x13>
        cnt ++;
  801560:	83 45 fc 01          	addl   $0x1,-0x4(%ebp)
 * The strlen() function returns the length of string @s.
 * */
size_t
strlen(const char *s) {
    size_t cnt = 0;
    while (*s ++ != '\0') {
  801564:	8b 45 08             	mov    0x8(%ebp),%eax
  801567:	8d 50 01             	lea    0x1(%eax),%edx
  80156a:	89 55 08             	mov    %edx,0x8(%ebp)
  80156d:	0f b6 00             	movzbl (%eax),%eax
  801570:	84 c0                	test   %al,%al
  801572:	75 ec                	jne    801560 <strlen+0xf>
        cnt ++;
    }
    return cnt;
  801574:	8b 45 fc             	mov    -0x4(%ebp),%eax
}
  801577:	c9                   	leave  
  801578:	c3                   	ret    

00801579 <strnlen>:
 * The return value is strlen(s), if that is less than @len, or
 * @len if there is no '\0' character among the first @len characters
 * pointed by @s.
 * */
size_t
strnlen(const char *s, size_t len) {
  801579:	55                   	push   %ebp
  80157a:	89 e5                	mov    %esp,%ebp
  80157c:	83 ec 10             	sub    $0x10,%esp
    size_t cnt = 0;
  80157f:	c7 45 fc 00 00 00 00 	movl   $0x0,-0x4(%ebp)
    while (cnt < len && *s ++ != '\0') {
  801586:	eb 04                	jmp    80158c <strnlen+0x13>
        cnt ++;
  801588:	83 45 fc 01          	addl   $0x1,-0x4(%ebp)
 * pointed by @s.
 * */
size_t
strnlen(const char *s, size_t len) {
    size_t cnt = 0;
    while (cnt < len && *s ++ != '\0') {
  80158c:	8b 45 fc             	mov    -0x4(%ebp),%eax
  80158f:	3b 45 0c             	cmp    0xc(%ebp),%eax
  801592:	73 10                	jae    8015a4 <strnlen+0x2b>
  801594:	8b 45 08             	mov    0x8(%ebp),%eax
  801597:	8d 50 01             	lea    0x1(%eax),%edx
  80159a:	89 55 08             	mov    %edx,0x8(%ebp)
  80159d:	0f b6 00             	movzbl (%eax),%eax
  8015a0:	84 c0                	test   %al,%al
  8015a2:	75 e4                	jne    801588 <strnlen+0xf>
        cnt ++;
    }
    return cnt;
  8015a4:	8b 45 fc             	mov    -0x4(%ebp),%eax
}
  8015a7:	c9                   	leave  
  8015a8:	c3                   	ret    

008015a9 <strcat>:
 * @dst:    pointer to the @dst array, which should be large enough to contain the concatenated
 *          resulting string.
 * @src:    string to be appended, this should not overlap @dst
 * */
char *
strcat(char *dst, const char *src) {
  8015a9:	55                   	push   %ebp
  8015aa:	89 e5                	mov    %esp,%ebp
  8015ac:	83 ec 18             	sub    $0x18,%esp
    return strcpy(dst + strlen(dst), src);
  8015af:	8b 45 08             	mov    0x8(%ebp),%eax
  8015b2:	89 04 24             	mov    %eax,(%esp)
  8015b5:	e8 97 ff ff ff       	call   801551 <strlen>
  8015ba:	8b 55 08             	mov    0x8(%ebp),%edx
  8015bd:	01 c2                	add    %eax,%edx
  8015bf:	8b 45 0c             	mov    0xc(%ebp),%eax
  8015c2:	89 44 24 04          	mov    %eax,0x4(%esp)
  8015c6:	89 14 24             	mov    %edx,(%esp)
  8015c9:	e8 02 00 00 00       	call   8015d0 <strcpy>
}
  8015ce:	c9                   	leave  
  8015cf:	c3                   	ret    

008015d0 <strcpy>:
 * To avoid overflows, the size of array pointed by @dst should be long enough to
 * contain the same string as @src (including the terminating null character), and
 * should not overlap in memory with @src.
 * */
char *
strcpy(char *dst, const char *src) {
  8015d0:	55                   	push   %ebp
  8015d1:	89 e5                	mov    %esp,%ebp
  8015d3:	57                   	push   %edi
  8015d4:	56                   	push   %esi
  8015d5:	83 ec 20             	sub    $0x20,%esp
  8015d8:	8b 45 08             	mov    0x8(%ebp),%eax
  8015db:	89 45 f4             	mov    %eax,-0xc(%ebp)
  8015de:	8b 45 0c             	mov    0xc(%ebp),%eax
  8015e1:	89 45 f0             	mov    %eax,-0x10(%ebp)
#ifndef __HAVE_ARCH_STRCPY
#define __HAVE_ARCH_STRCPY
static inline char *
__strcpy(char *dst, const char *src) {
    int d0, d1, d2;
    asm volatile (
  8015e4:	8b 55 f0             	mov    -0x10(%ebp),%edx
  8015e7:	8b 45 f4             	mov    -0xc(%ebp),%eax
  8015ea:	89 d1                	mov    %edx,%ecx
  8015ec:	89 c2                	mov    %eax,%edx
  8015ee:	89 ce                	mov    %ecx,%esi
  8015f0:	89 d7                	mov    %edx,%edi
  8015f2:	ac                   	lods   %ds:(%esi),%al
  8015f3:	aa                   	stos   %al,%es:(%edi)
  8015f4:	84 c0                	test   %al,%al
  8015f6:	75 fa                	jne    8015f2 <strcpy+0x22>
  8015f8:	89 fa                	mov    %edi,%edx
  8015fa:	89 f1                	mov    %esi,%ecx
  8015fc:	89 4d ec             	mov    %ecx,-0x14(%ebp)
  8015ff:	89 55 e8             	mov    %edx,-0x18(%ebp)
  801602:	89 45 e4             	mov    %eax,-0x1c(%ebp)
        "stosb;"
        "testb %%al, %%al;"
        "jne 1b;"
        : "=&S" (d0), "=&D" (d1), "=&a" (d2)
        : "0" (src), "1" (dst) : "memory");
    return dst;
  801605:	8b 45 f4             	mov    -0xc(%ebp),%eax
    char *p = dst;
    while ((*p ++ = *src ++) != '\0')
        /* nothing */;
    return dst;
#endif /* __HAVE_ARCH_STRCPY */
}
  801608:	83 c4 20             	add    $0x20,%esp
  80160b:	5e                   	pop    %esi
  80160c:	5f                   	pop    %edi
  80160d:	5d                   	pop    %ebp
  80160e:	c3                   	ret    

0080160f <strncpy>:
 * @len:    maximum number of characters to be copied from @src
 *
 * The return value is @dst
 * */
char *
strncpy(char *dst, const char *src, size_t len) {
  80160f:	55                   	push   %ebp
  801610:	89 e5                	mov    %esp,%ebp
  801612:	83 ec 10             	sub    $0x10,%esp
    char *p = dst;
  801615:	8b 45 08             	mov    0x8(%ebp),%eax
  801618:	89 45 fc             	mov    %eax,-0x4(%ebp)
    while (len > 0) {
  80161b:	eb 21                	jmp    80163e <strncpy+0x2f>
        if ((*p = *src) != '\0') {
  80161d:	8b 45 0c             	mov    0xc(%ebp),%eax
  801620:	0f b6 10             	movzbl (%eax),%edx
  801623:	8b 45 fc             	mov    -0x4(%ebp),%eax
  801626:	88 10                	mov    %dl,(%eax)
  801628:	8b 45 fc             	mov    -0x4(%ebp),%eax
  80162b:	0f b6 00             	movzbl (%eax),%eax
  80162e:	84 c0                	test   %al,%al
  801630:	74 04                	je     801636 <strncpy+0x27>
            src ++;
  801632:	83 45 0c 01          	addl   $0x1,0xc(%ebp)
        }
        p ++, len --;
  801636:	83 45 fc 01          	addl   $0x1,-0x4(%ebp)
  80163a:	83 6d 10 01          	subl   $0x1,0x10(%ebp)
 * The return value is @dst
 * */
char *
strncpy(char *dst, const char *src, size_t len) {
    char *p = dst;
    while (len > 0) {
  80163e:	83 7d 10 00          	cmpl   $0x0,0x10(%ebp)
  801642:	75 d9                	jne    80161d <strncpy+0xe>
        if ((*p = *src) != '\0') {
            src ++;
        }
        p ++, len --;
    }
    return dst;
  801644:	8b 45 08             	mov    0x8(%ebp),%eax
}
  801647:	c9                   	leave  
  801648:	c3                   	ret    

00801649 <strcmp>:
 * - A value greater than zero indicates that the first character that does
 *   not match has a greater value in @s1 than in @s2;
 * - And a value less than zero indicates the opposite.
 * */
int
strcmp(const char *s1, const char *s2) {
  801649:	55                   	push   %ebp
  80164a:	89 e5                	mov    %esp,%ebp
  80164c:	57                   	push   %edi
  80164d:	56                   	push   %esi
  80164e:	83 ec 20             	sub    $0x20,%esp
  801651:	8b 45 08             	mov    0x8(%ebp),%eax
  801654:	89 45 f4             	mov    %eax,-0xc(%ebp)
  801657:	8b 45 0c             	mov    0xc(%ebp),%eax
  80165a:	89 45 f0             	mov    %eax,-0x10(%ebp)
#ifndef __HAVE_ARCH_STRCMP
#define __HAVE_ARCH_STRCMP
static inline int
__strcmp(const char *s1, const char *s2) {
    int d0, d1, ret;
    asm volatile (
  80165d:	8b 55 f4             	mov    -0xc(%ebp),%edx
  801660:	8b 45 f0             	mov    -0x10(%ebp),%eax
  801663:	89 d1                	mov    %edx,%ecx
  801665:	89 c2                	mov    %eax,%edx
  801667:	89 ce                	mov    %ecx,%esi
  801669:	89 d7                	mov    %edx,%edi
  80166b:	ac                   	lods   %ds:(%esi),%al
  80166c:	ae                   	scas   %es:(%edi),%al
  80166d:	75 08                	jne    801677 <strcmp+0x2e>
  80166f:	84 c0                	test   %al,%al
  801671:	75 f8                	jne    80166b <strcmp+0x22>
  801673:	31 c0                	xor    %eax,%eax
  801675:	eb 04                	jmp    80167b <strcmp+0x32>
  801677:	19 c0                	sbb    %eax,%eax
  801679:	0c 01                	or     $0x1,%al
  80167b:	89 fa                	mov    %edi,%edx
  80167d:	89 f1                	mov    %esi,%ecx
  80167f:	89 45 ec             	mov    %eax,-0x14(%ebp)
  801682:	89 4d e8             	mov    %ecx,-0x18(%ebp)
  801685:	89 55 e4             	mov    %edx,-0x1c(%ebp)
        "orb $1, %%al;"
        "3:"
        : "=a" (ret), "=&S" (d0), "=&D" (d1)
        : "1" (s1), "2" (s2)
        : "memory");
    return ret;
  801688:	8b 45 ec             	mov    -0x14(%ebp),%eax
    while (*s1 != '\0' && *s1 == *s2) {
        s1 ++, s2 ++;
    }
    return (int)((unsigned char)*s1 - (unsigned char)*s2);
#endif /* __HAVE_ARCH_STRCMP */
}
  80168b:	83 c4 20             	add    $0x20,%esp
  80168e:	5e                   	pop    %esi
  80168f:	5f                   	pop    %edi
  801690:	5d                   	pop    %ebp
  801691:	c3                   	ret    

00801692 <strncmp>:
 * they are equal to each other, it continues with the following pairs until
 * the characters differ, until a terminating null-character is reached, or
 * until @n characters match in both strings, whichever happens first.
 * */
int
strncmp(const char *s1, const char *s2, size_t n) {
  801692:	55                   	push   %ebp
  801693:	89 e5                	mov    %esp,%ebp
    while (n > 0 && *s1 != '\0' && *s1 == *s2) {
  801695:	eb 0c                	jmp    8016a3 <strncmp+0x11>
        n --, s1 ++, s2 ++;
  801697:	83 6d 10 01          	subl   $0x1,0x10(%ebp)
  80169b:	83 45 08 01          	addl   $0x1,0x8(%ebp)
  80169f:	83 45 0c 01          	addl   $0x1,0xc(%ebp)
 * the characters differ, until a terminating null-character is reached, or
 * until @n characters match in both strings, whichever happens first.
 * */
int
strncmp(const char *s1, const char *s2, size_t n) {
    while (n > 0 && *s1 != '\0' && *s1 == *s2) {
  8016a3:	83 7d 10 00          	cmpl   $0x0,0x10(%ebp)
  8016a7:	74 1a                	je     8016c3 <strncmp+0x31>
  8016a9:	8b 45 08             	mov    0x8(%ebp),%eax
  8016ac:	0f b6 00             	movzbl (%eax),%eax
  8016af:	84 c0                	test   %al,%al
  8016b1:	74 10                	je     8016c3 <strncmp+0x31>
  8016b3:	8b 45 08             	mov    0x8(%ebp),%eax
  8016b6:	0f b6 10             	movzbl (%eax),%edx
  8016b9:	8b 45 0c             	mov    0xc(%ebp),%eax
  8016bc:	0f b6 00             	movzbl (%eax),%eax
  8016bf:	38 c2                	cmp    %al,%dl
  8016c1:	74 d4                	je     801697 <strncmp+0x5>
        n --, s1 ++, s2 ++;
    }
    return (n == 0) ? 0 : (int)((unsigned char)*s1 - (unsigned char)*s2);
  8016c3:	83 7d 10 00          	cmpl   $0x0,0x10(%ebp)
  8016c7:	74 18                	je     8016e1 <strncmp+0x4f>
  8016c9:	8b 45 08             	mov    0x8(%ebp),%eax
  8016cc:	0f b6 00             	movzbl (%eax),%eax
  8016cf:	0f b6 d0             	movzbl %al,%edx
  8016d2:	8b 45 0c             	mov    0xc(%ebp),%eax
  8016d5:	0f b6 00             	movzbl (%eax),%eax
  8016d8:	0f b6 c0             	movzbl %al,%eax
  8016db:	29 c2                	sub    %eax,%edx
  8016dd:	89 d0                	mov    %edx,%eax
  8016df:	eb 05                	jmp    8016e6 <strncmp+0x54>
  8016e1:	b8 00 00 00 00       	mov    $0x0,%eax
}
  8016e6:	5d                   	pop    %ebp
  8016e7:	c3                   	ret    

008016e8 <strchr>:
 *
 * The strchr() function returns a pointer to the first occurrence of
 * character in @s. If the value is not found, the function returns 'NULL'.
 * */
char *
strchr(const char *s, char c) {
  8016e8:	55                   	push   %ebp
  8016e9:	89 e5                	mov    %esp,%ebp
  8016eb:	83 ec 04             	sub    $0x4,%esp
  8016ee:	8b 45 0c             	mov    0xc(%ebp),%eax
  8016f1:	88 45 fc             	mov    %al,-0x4(%ebp)
    while (*s != '\0') {
  8016f4:	eb 14                	jmp    80170a <strchr+0x22>
        if (*s == c) {
  8016f6:	8b 45 08             	mov    0x8(%ebp),%eax
  8016f9:	0f b6 00             	movzbl (%eax),%eax
  8016fc:	3a 45 fc             	cmp    -0x4(%ebp),%al
  8016ff:	75 05                	jne    801706 <strchr+0x1e>
            return (char *)s;
  801701:	8b 45 08             	mov    0x8(%ebp),%eax
  801704:	eb 13                	jmp    801719 <strchr+0x31>
        }
        s ++;
  801706:	83 45 08 01          	addl   $0x1,0x8(%ebp)
 * The strchr() function returns a pointer to the first occurrence of
 * character in @s. If the value is not found, the function returns 'NULL'.
 * */
char *
strchr(const char *s, char c) {
    while (*s != '\0') {
  80170a:	8b 45 08             	mov    0x8(%ebp),%eax
  80170d:	0f b6 00             	movzbl (%eax),%eax
  801710:	84 c0                	test   %al,%al
  801712:	75 e2                	jne    8016f6 <strchr+0xe>
        if (*s == c) {
            return (char *)s;
        }
        s ++;
    }
    return NULL;
  801714:	b8 00 00 00 00       	mov    $0x0,%eax
}
  801719:	c9                   	leave  
  80171a:	c3                   	ret    

0080171b <strfind>:
 * The strfind() function is like strchr() except that if @c is
 * not found in @s, then it returns a pointer to the null byte at the
 * end of @s, rather than 'NULL'.
 * */
char *
strfind(const char *s, char c) {
  80171b:	55                   	push   %ebp
  80171c:	89 e5                	mov    %esp,%ebp
  80171e:	83 ec 04             	sub    $0x4,%esp
  801721:	8b 45 0c             	mov    0xc(%ebp),%eax
  801724:	88 45 fc             	mov    %al,-0x4(%ebp)
    while (*s != '\0') {
  801727:	eb 11                	jmp    80173a <strfind+0x1f>
        if (*s == c) {
  801729:	8b 45 08             	mov    0x8(%ebp),%eax
  80172c:	0f b6 00             	movzbl (%eax),%eax
  80172f:	3a 45 fc             	cmp    -0x4(%ebp),%al
  801732:	75 02                	jne    801736 <strfind+0x1b>
            break;
  801734:	eb 0e                	jmp    801744 <strfind+0x29>
        }
        s ++;
  801736:	83 45 08 01          	addl   $0x1,0x8(%ebp)
 * not found in @s, then it returns a pointer to the null byte at the
 * end of @s, rather than 'NULL'.
 * */
char *
strfind(const char *s, char c) {
    while (*s != '\0') {
  80173a:	8b 45 08             	mov    0x8(%ebp),%eax
  80173d:	0f b6 00             	movzbl (%eax),%eax
  801740:	84 c0                	test   %al,%al
  801742:	75 e5                	jne    801729 <strfind+0xe>
        if (*s == c) {
            break;
        }
        s ++;
    }
    return (char *)s;
  801744:	8b 45 08             	mov    0x8(%ebp),%eax
}
  801747:	c9                   	leave  
  801748:	c3                   	ret    

00801749 <strtol>:
 * an optional "0x" or "0X" prefix.
 *
 * The strtol() function returns the converted integral number as a long int value.
 * */
long
strtol(const char *s, char **endptr, int base) {
  801749:	55                   	push   %ebp
  80174a:	89 e5                	mov    %esp,%ebp
  80174c:	83 ec 10             	sub    $0x10,%esp
    int neg = 0;
  80174f:	c7 45 fc 00 00 00 00 	movl   $0x0,-0x4(%ebp)
    long val = 0;
  801756:	c7 45 f8 00 00 00 00 	movl   $0x0,-0x8(%ebp)

    // gobble initial whitespace
    while (*s == ' ' || *s == '\t') {
  80175d:	eb 04                	jmp    801763 <strtol+0x1a>
        s ++;
  80175f:	83 45 08 01          	addl   $0x1,0x8(%ebp)
strtol(const char *s, char **endptr, int base) {
    int neg = 0;
    long val = 0;

    // gobble initial whitespace
    while (*s == ' ' || *s == '\t') {
  801763:	8b 45 08             	mov    0x8(%ebp),%eax
  801766:	0f b6 00             	movzbl (%eax),%eax
  801769:	3c 20                	cmp    $0x20,%al
  80176b:	74 f2                	je     80175f <strtol+0x16>
  80176d:	8b 45 08             	mov    0x8(%ebp),%eax
  801770:	0f b6 00             	movzbl (%eax),%eax
  801773:	3c 09                	cmp    $0x9,%al
  801775:	74 e8                	je     80175f <strtol+0x16>
        s ++;
    }

    // plus/minus sign
    if (*s == '+') {
  801777:	8b 45 08             	mov    0x8(%ebp),%eax
  80177a:	0f b6 00             	movzbl (%eax),%eax
  80177d:	3c 2b                	cmp    $0x2b,%al
  80177f:	75 06                	jne    801787 <strtol+0x3e>
        s ++;
  801781:	83 45 08 01          	addl   $0x1,0x8(%ebp)
  801785:	eb 15                	jmp    80179c <strtol+0x53>
    }
    else if (*s == '-') {
  801787:	8b 45 08             	mov    0x8(%ebp),%eax
  80178a:	0f b6 00             	movzbl (%eax),%eax
  80178d:	3c 2d                	cmp    $0x2d,%al
  80178f:	75 0b                	jne    80179c <strtol+0x53>
        s ++, neg = 1;
  801791:	83 45 08 01          	addl   $0x1,0x8(%ebp)
  801795:	c7 45 fc 01 00 00 00 	movl   $0x1,-0x4(%ebp)
    }

    // hex or octal base prefix
    if ((base == 0 || base == 16) && (s[0] == '0' && s[1] == 'x')) {
  80179c:	83 7d 10 00          	cmpl   $0x0,0x10(%ebp)
  8017a0:	74 06                	je     8017a8 <strtol+0x5f>
  8017a2:	83 7d 10 10          	cmpl   $0x10,0x10(%ebp)
  8017a6:	75 24                	jne    8017cc <strtol+0x83>
  8017a8:	8b 45 08             	mov    0x8(%ebp),%eax
  8017ab:	0f b6 00             	movzbl (%eax),%eax
  8017ae:	3c 30                	cmp    $0x30,%al
  8017b0:	75 1a                	jne    8017cc <strtol+0x83>
  8017b2:	8b 45 08             	mov    0x8(%ebp),%eax
  8017b5:	83 c0 01             	add    $0x1,%eax
  8017b8:	0f b6 00             	movzbl (%eax),%eax
  8017bb:	3c 78                	cmp    $0x78,%al
  8017bd:	75 0d                	jne    8017cc <strtol+0x83>
        s += 2, base = 16;
  8017bf:	83 45 08 02          	addl   $0x2,0x8(%ebp)
  8017c3:	c7 45 10 10 00 00 00 	movl   $0x10,0x10(%ebp)
  8017ca:	eb 2a                	jmp    8017f6 <strtol+0xad>
    }
    else if (base == 0 && s[0] == '0') {
  8017cc:	83 7d 10 00          	cmpl   $0x0,0x10(%ebp)
  8017d0:	75 17                	jne    8017e9 <strtol+0xa0>
  8017d2:	8b 45 08             	mov    0x8(%ebp),%eax
  8017d5:	0f b6 00             	movzbl (%eax),%eax
  8017d8:	3c 30                	cmp    $0x30,%al
  8017da:	75 0d                	jne    8017e9 <strtol+0xa0>
        s ++, base = 8;
  8017dc:	83 45 08 01          	addl   $0x1,0x8(%ebp)
  8017e0:	c7 45 10 08 00 00 00 	movl   $0x8,0x10(%ebp)
  8017e7:	eb 0d                	jmp    8017f6 <strtol+0xad>
    }
    else if (base == 0) {
  8017e9:	83 7d 10 00          	cmpl   $0x0,0x10(%ebp)
  8017ed:	75 07                	jne    8017f6 <strtol+0xad>
        base = 10;
  8017ef:	c7 45 10 0a 00 00 00 	movl   $0xa,0x10(%ebp)

    // digits
    while (1) {
        int dig;

        if (*s >= '0' && *s <= '9') {
  8017f6:	8b 45 08             	mov    0x8(%ebp),%eax
  8017f9:	0f b6 00             	movzbl (%eax),%eax
  8017fc:	3c 2f                	cmp    $0x2f,%al
  8017fe:	7e 1b                	jle    80181b <strtol+0xd2>
  801800:	8b 45 08             	mov    0x8(%ebp),%eax
  801803:	0f b6 00             	movzbl (%eax),%eax
  801806:	3c 39                	cmp    $0x39,%al
  801808:	7f 11                	jg     80181b <strtol+0xd2>
            dig = *s - '0';
  80180a:	8b 45 08             	mov    0x8(%ebp),%eax
  80180d:	0f b6 00             	movzbl (%eax),%eax
  801810:	0f be c0             	movsbl %al,%eax
  801813:	83 e8 30             	sub    $0x30,%eax
  801816:	89 45 f4             	mov    %eax,-0xc(%ebp)
  801819:	eb 48                	jmp    801863 <strtol+0x11a>
        }
        else if (*s >= 'a' && *s <= 'z') {
  80181b:	8b 45 08             	mov    0x8(%ebp),%eax
  80181e:	0f b6 00             	movzbl (%eax),%eax
  801821:	3c 60                	cmp    $0x60,%al
  801823:	7e 1b                	jle    801840 <strtol+0xf7>
  801825:	8b 45 08             	mov    0x8(%ebp),%eax
  801828:	0f b6 00             	movzbl (%eax),%eax
  80182b:	3c 7a                	cmp    $0x7a,%al
  80182d:	7f 11                	jg     801840 <strtol+0xf7>
            dig = *s - 'a' + 10;
  80182f:	8b 45 08             	mov    0x8(%ebp),%eax
  801832:	0f b6 00             	movzbl (%eax),%eax
  801835:	0f be c0             	movsbl %al,%eax
  801838:	83 e8 57             	sub    $0x57,%eax
  80183b:	89 45 f4             	mov    %eax,-0xc(%ebp)
  80183e:	eb 23                	jmp    801863 <strtol+0x11a>
        }
        else if (*s >= 'A' && *s <= 'Z') {
  801840:	8b 45 08             	mov    0x8(%ebp),%eax
  801843:	0f b6 00             	movzbl (%eax),%eax
  801846:	3c 40                	cmp    $0x40,%al
  801848:	7e 3d                	jle    801887 <strtol+0x13e>
  80184a:	8b 45 08             	mov    0x8(%ebp),%eax
  80184d:	0f b6 00             	movzbl (%eax),%eax
  801850:	3c 5a                	cmp    $0x5a,%al
  801852:	7f 33                	jg     801887 <strtol+0x13e>
            dig = *s - 'A' + 10;
  801854:	8b 45 08             	mov    0x8(%ebp),%eax
  801857:	0f b6 00             	movzbl (%eax),%eax
  80185a:	0f be c0             	movsbl %al,%eax
  80185d:	83 e8 37             	sub    $0x37,%eax
  801860:	89 45 f4             	mov    %eax,-0xc(%ebp)
        }
        else {
            break;
        }
        if (dig >= base) {
  801863:	8b 45 f4             	mov    -0xc(%ebp),%eax
  801866:	3b 45 10             	cmp    0x10(%ebp),%eax
  801869:	7c 02                	jl     80186d <strtol+0x124>
            break;
  80186b:	eb 1a                	jmp    801887 <strtol+0x13e>
        }
        s ++, val = (val * base) + dig;
  80186d:	83 45 08 01          	addl   $0x1,0x8(%ebp)
  801871:	8b 45 f8             	mov    -0x8(%ebp),%eax
  801874:	0f af 45 10          	imul   0x10(%ebp),%eax
  801878:	89 c2                	mov    %eax,%edx
  80187a:	8b 45 f4             	mov    -0xc(%ebp),%eax
  80187d:	01 d0                	add    %edx,%eax
  80187f:	89 45 f8             	mov    %eax,-0x8(%ebp)
        // we don't properly detect overflow!
    }
  801882:	e9 6f ff ff ff       	jmp    8017f6 <strtol+0xad>

    if (endptr) {
  801887:	83 7d 0c 00          	cmpl   $0x0,0xc(%ebp)
  80188b:	74 08                	je     801895 <strtol+0x14c>
        *endptr = (char *) s;
  80188d:	8b 45 0c             	mov    0xc(%ebp),%eax
  801890:	8b 55 08             	mov    0x8(%ebp),%edx
  801893:	89 10                	mov    %edx,(%eax)
    }
    return (neg ? -val : val);
  801895:	83 7d fc 00          	cmpl   $0x0,-0x4(%ebp)
  801899:	74 07                	je     8018a2 <strtol+0x159>
  80189b:	8b 45 f8             	mov    -0x8(%ebp),%eax
  80189e:	f7 d8                	neg    %eax
  8018a0:	eb 03                	jmp    8018a5 <strtol+0x15c>
  8018a2:	8b 45 f8             	mov    -0x8(%ebp),%eax
}
  8018a5:	c9                   	leave  
  8018a6:	c3                   	ret    

008018a7 <memset>:
 * @n:      number of bytes to be set to the value
 *
 * The memset() function returns @s.
 * */
void *
memset(void *s, char c, size_t n) {
  8018a7:	55                   	push   %ebp
  8018a8:	89 e5                	mov    %esp,%ebp
  8018aa:	57                   	push   %edi
  8018ab:	83 ec 24             	sub    $0x24,%esp
  8018ae:	8b 45 0c             	mov    0xc(%ebp),%eax
  8018b1:	88 45 d8             	mov    %al,-0x28(%ebp)
#ifdef __HAVE_ARCH_MEMSET
    return __memset(s, c, n);
  8018b4:	0f be 45 d8          	movsbl -0x28(%ebp),%eax
  8018b8:	8b 55 08             	mov    0x8(%ebp),%edx
  8018bb:	89 55 f8             	mov    %edx,-0x8(%ebp)
  8018be:	88 45 f7             	mov    %al,-0x9(%ebp)
  8018c1:	8b 45 10             	mov    0x10(%ebp),%eax
  8018c4:	89 45 f0             	mov    %eax,-0x10(%ebp)
#ifndef __HAVE_ARCH_MEMSET
#define __HAVE_ARCH_MEMSET
static inline void *
__memset(void *s, char c, size_t n) {
    int d0, d1;
    asm volatile (
  8018c7:	8b 4d f0             	mov    -0x10(%ebp),%ecx
  8018ca:	0f b6 45 f7          	movzbl -0x9(%ebp),%eax
  8018ce:	8b 55 f8             	mov    -0x8(%ebp),%edx
  8018d1:	89 d7                	mov    %edx,%edi
  8018d3:	f3 aa                	rep stos %al,%es:(%edi)
  8018d5:	89 fa                	mov    %edi,%edx
  8018d7:	89 4d ec             	mov    %ecx,-0x14(%ebp)
  8018da:	89 55 e8             	mov    %edx,-0x18(%ebp)
        "rep; stosb;"
        : "=&c" (d0), "=&D" (d1)
        : "0" (n), "a" (c), "1" (s)
        : "memory");
    return s;
  8018dd:	8b 45 f8             	mov    -0x8(%ebp),%eax
    while (n -- > 0) {
        *p ++ = c;
    }
    return s;
#endif /* __HAVE_ARCH_MEMSET */
}
  8018e0:	83 c4 24             	add    $0x24,%esp
  8018e3:	5f                   	pop    %edi
  8018e4:	5d                   	pop    %ebp
  8018e5:	c3                   	ret    

008018e6 <memmove>:
 * @n:      number of bytes to copy
 *
 * The memmove() function returns @dst.
 * */
void *
memmove(void *dst, const void *src, size_t n) {
  8018e6:	55                   	push   %ebp
  8018e7:	89 e5                	mov    %esp,%ebp
  8018e9:	57                   	push   %edi
  8018ea:	56                   	push   %esi
  8018eb:	53                   	push   %ebx
  8018ec:	83 ec 30             	sub    $0x30,%esp
  8018ef:	8b 45 08             	mov    0x8(%ebp),%eax
  8018f2:	89 45 f0             	mov    %eax,-0x10(%ebp)
  8018f5:	8b 45 0c             	mov    0xc(%ebp),%eax
  8018f8:	89 45 ec             	mov    %eax,-0x14(%ebp)
  8018fb:	8b 45 10             	mov    0x10(%ebp),%eax
  8018fe:	89 45 e8             	mov    %eax,-0x18(%ebp)

#ifndef __HAVE_ARCH_MEMMOVE
#define __HAVE_ARCH_MEMMOVE
static inline void *
__memmove(void *dst, const void *src, size_t n) {
    if (dst < src) {
  801901:	8b 45 f0             	mov    -0x10(%ebp),%eax
  801904:	3b 45 ec             	cmp    -0x14(%ebp),%eax
  801907:	73 42                	jae    80194b <memmove+0x65>
  801909:	8b 45 f0             	mov    -0x10(%ebp),%eax
  80190c:	89 45 e4             	mov    %eax,-0x1c(%ebp)
  80190f:	8b 45 ec             	mov    -0x14(%ebp),%eax
  801912:	89 45 e0             	mov    %eax,-0x20(%ebp)
  801915:	8b 45 e8             	mov    -0x18(%ebp),%eax
  801918:	89 45 dc             	mov    %eax,-0x24(%ebp)
        "andl $3, %%ecx;"
        "jz 1f;"
        "rep; movsb;"
        "1:"
        : "=&c" (d0), "=&D" (d1), "=&S" (d2)
        : "0" (n / 4), "g" (n), "1" (dst), "2" (src)
  80191b:	8b 45 dc             	mov    -0x24(%ebp),%eax
  80191e:	c1 e8 02             	shr    $0x2,%eax
  801921:	89 c1                	mov    %eax,%ecx
#ifndef __HAVE_ARCH_MEMCPY
#define __HAVE_ARCH_MEMCPY
static inline void *
__memcpy(void *dst, const void *src, size_t n) {
    int d0, d1, d2;
    asm volatile (
  801923:	8b 55 e4             	mov    -0x1c(%ebp),%edx
  801926:	8b 45 e0             	mov    -0x20(%ebp),%eax
  801929:	89 d7                	mov    %edx,%edi
  80192b:	89 c6                	mov    %eax,%esi
  80192d:	f3 a5                	rep movsl %ds:(%esi),%es:(%edi)
  80192f:	8b 4d dc             	mov    -0x24(%ebp),%ecx
  801932:	83 e1 03             	and    $0x3,%ecx
  801935:	74 02                	je     801939 <memmove+0x53>
  801937:	f3 a4                	rep movsb %ds:(%esi),%es:(%edi)
  801939:	89 f0                	mov    %esi,%eax
  80193b:	89 fa                	mov    %edi,%edx
  80193d:	89 4d d8             	mov    %ecx,-0x28(%ebp)
  801940:	89 55 d4             	mov    %edx,-0x2c(%ebp)
  801943:	89 45 d0             	mov    %eax,-0x30(%ebp)
        "rep; movsb;"
        "1:"
        : "=&c" (d0), "=&D" (d1), "=&S" (d2)
        : "0" (n / 4), "g" (n), "1" (dst), "2" (src)
        : "memory");
    return dst;
  801946:	8b 45 e4             	mov    -0x1c(%ebp),%eax
  801949:	eb 36                	jmp    801981 <memmove+0x9b>
    asm volatile (
        "std;"
        "rep; movsb;"
        "cld;"
        : "=&c" (d0), "=&S" (d1), "=&D" (d2)
        : "0" (n), "1" (n - 1 + src), "2" (n - 1 + dst)
  80194b:	8b 45 e8             	mov    -0x18(%ebp),%eax
  80194e:	8d 50 ff             	lea    -0x1(%eax),%edx
  801951:	8b 45 ec             	mov    -0x14(%ebp),%eax
  801954:	01 c2                	add    %eax,%edx
  801956:	8b 45 e8             	mov    -0x18(%ebp),%eax
  801959:	8d 48 ff             	lea    -0x1(%eax),%ecx
  80195c:	8b 45 f0             	mov    -0x10(%ebp),%eax
  80195f:	8d 1c 01             	lea    (%ecx,%eax,1),%ebx
__memmove(void *dst, const void *src, size_t n) {
    if (dst < src) {
        return __memcpy(dst, src, n);
    }
    int d0, d1, d2;
    asm volatile (
  801962:	8b 45 e8             	mov    -0x18(%ebp),%eax
  801965:	89 c1                	mov    %eax,%ecx
  801967:	89 d8                	mov    %ebx,%eax
  801969:	89 d6                	mov    %edx,%esi
  80196b:	89 c7                	mov    %eax,%edi
  80196d:	fd                   	std    
  80196e:	f3 a4                	rep movsb %ds:(%esi),%es:(%edi)
  801970:	fc                   	cld    
  801971:	89 f8                	mov    %edi,%eax
  801973:	89 f2                	mov    %esi,%edx
  801975:	89 4d cc             	mov    %ecx,-0x34(%ebp)
  801978:	89 55 c8             	mov    %edx,-0x38(%ebp)
  80197b:	89 45 c4             	mov    %eax,-0x3c(%ebp)
        "rep; movsb;"
        "cld;"
        : "=&c" (d0), "=&S" (d1), "=&D" (d2)
        : "0" (n), "1" (n - 1 + src), "2" (n - 1 + dst)
        : "memory");
    return dst;
  80197e:	8b 45 f0             	mov    -0x10(%ebp),%eax
            *d ++ = *s ++;
        }
    }
    return dst;
#endif /* __HAVE_ARCH_MEMMOVE */
}
  801981:	83 c4 30             	add    $0x30,%esp
  801984:	5b                   	pop    %ebx
  801985:	5e                   	pop    %esi
  801986:	5f                   	pop    %edi
  801987:	5d                   	pop    %ebp
  801988:	c3                   	ret    

00801989 <memcpy>:
 * it always copies exactly @n bytes. To avoid overflows, the size of arrays pointed
 * by both @src and @dst, should be at least @n bytes, and should not overlap
 * (for overlapping memory area, memmove is a safer approach).
 * */
void *
memcpy(void *dst, const void *src, size_t n) {
  801989:	55                   	push   %ebp
  80198a:	89 e5                	mov    %esp,%ebp
  80198c:	57                   	push   %edi
  80198d:	56                   	push   %esi
  80198e:	83 ec 20             	sub    $0x20,%esp
  801991:	8b 45 08             	mov    0x8(%ebp),%eax
  801994:	89 45 f4             	mov    %eax,-0xc(%ebp)
  801997:	8b 45 0c             	mov    0xc(%ebp),%eax
  80199a:	89 45 f0             	mov    %eax,-0x10(%ebp)
  80199d:	8b 45 10             	mov    0x10(%ebp),%eax
  8019a0:	89 45 ec             	mov    %eax,-0x14(%ebp)
        "andl $3, %%ecx;"
        "jz 1f;"
        "rep; movsb;"
        "1:"
        : "=&c" (d0), "=&D" (d1), "=&S" (d2)
        : "0" (n / 4), "g" (n), "1" (dst), "2" (src)
  8019a3:	8b 45 ec             	mov    -0x14(%ebp),%eax
  8019a6:	c1 e8 02             	shr    $0x2,%eax
  8019a9:	89 c1                	mov    %eax,%ecx
#ifndef __HAVE_ARCH_MEMCPY
#define __HAVE_ARCH_MEMCPY
static inline void *
__memcpy(void *dst, const void *src, size_t n) {
    int d0, d1, d2;
    asm volatile (
  8019ab:	8b 55 f4             	mov    -0xc(%ebp),%edx
  8019ae:	8b 45 f0             	mov    -0x10(%ebp),%eax
  8019b1:	89 d7                	mov    %edx,%edi
  8019b3:	89 c6                	mov    %eax,%esi
  8019b5:	f3 a5                	rep movsl %ds:(%esi),%es:(%edi)
  8019b7:	8b 4d ec             	mov    -0x14(%ebp),%ecx
  8019ba:	83 e1 03             	and    $0x3,%ecx
  8019bd:	74 02                	je     8019c1 <memcpy+0x38>
  8019bf:	f3 a4                	rep movsb %ds:(%esi),%es:(%edi)
  8019c1:	89 f0                	mov    %esi,%eax
  8019c3:	89 fa                	mov    %edi,%edx
  8019c5:	89 4d e8             	mov    %ecx,-0x18(%ebp)
  8019c8:	89 55 e4             	mov    %edx,-0x1c(%ebp)
  8019cb:	89 45 e0             	mov    %eax,-0x20(%ebp)
        "rep; movsb;"
        "1:"
        : "=&c" (d0), "=&D" (d1), "=&S" (d2)
        : "0" (n / 4), "g" (n), "1" (dst), "2" (src)
        : "memory");
    return dst;
  8019ce:	8b 45 f4             	mov    -0xc(%ebp),%eax
    while (n -- > 0) {
        *d ++ = *s ++;
    }
    return dst;
#endif /* __HAVE_ARCH_MEMCPY */
}
  8019d1:	83 c4 20             	add    $0x20,%esp
  8019d4:	5e                   	pop    %esi
  8019d5:	5f                   	pop    %edi
  8019d6:	5d                   	pop    %ebp
  8019d7:	c3                   	ret    

008019d8 <memcmp>:
 *   match in both memory blocks has a greater value in @v1 than in @v2
 *   as if evaluated as unsigned char values;
 * - And a value less than zero indicates the opposite.
 * */
int
memcmp(const void *v1, const void *v2, size_t n) {
  8019d8:	55                   	push   %ebp
  8019d9:	89 e5                	mov    %esp,%ebp
  8019db:	83 ec 10             	sub    $0x10,%esp
    const char *s1 = (const char *)v1;
  8019de:	8b 45 08             	mov    0x8(%ebp),%eax
  8019e1:	89 45 fc             	mov    %eax,-0x4(%ebp)
    const char *s2 = (const char *)v2;
  8019e4:	8b 45 0c             	mov    0xc(%ebp),%eax
  8019e7:	89 45 f8             	mov    %eax,-0x8(%ebp)
    while (n -- > 0) {
  8019ea:	eb 30                	jmp    801a1c <memcmp+0x44>
        if (*s1 != *s2) {
  8019ec:	8b 45 fc             	mov    -0x4(%ebp),%eax
  8019ef:	0f b6 10             	movzbl (%eax),%edx
  8019f2:	8b 45 f8             	mov    -0x8(%ebp),%eax
  8019f5:	0f b6 00             	movzbl (%eax),%eax
  8019f8:	38 c2                	cmp    %al,%dl
  8019fa:	74 18                	je     801a14 <memcmp+0x3c>
            return (int)((unsigned char)*s1 - (unsigned char)*s2);
  8019fc:	8b 45 fc             	mov    -0x4(%ebp),%eax
  8019ff:	0f b6 00             	movzbl (%eax),%eax
  801a02:	0f b6 d0             	movzbl %al,%edx
  801a05:	8b 45 f8             	mov    -0x8(%ebp),%eax
  801a08:	0f b6 00             	movzbl (%eax),%eax
  801a0b:	0f b6 c0             	movzbl %al,%eax
  801a0e:	29 c2                	sub    %eax,%edx
  801a10:	89 d0                	mov    %edx,%eax
  801a12:	eb 1a                	jmp    801a2e <memcmp+0x56>
        }
        s1 ++, s2 ++;
  801a14:	83 45 fc 01          	addl   $0x1,-0x4(%ebp)
  801a18:	83 45 f8 01          	addl   $0x1,-0x8(%ebp)
 * */
int
memcmp(const void *v1, const void *v2, size_t n) {
    const char *s1 = (const char *)v1;
    const char *s2 = (const char *)v2;
    while (n -- > 0) {
  801a1c:	8b 45 10             	mov    0x10(%ebp),%eax
  801a1f:	8d 50 ff             	lea    -0x1(%eax),%edx
  801a22:	89 55 10             	mov    %edx,0x10(%ebp)
  801a25:	85 c0                	test   %eax,%eax
  801a27:	75 c3                	jne    8019ec <memcmp+0x14>
        if (*s1 != *s2) {
            return (int)((unsigned char)*s1 - (unsigned char)*s2);
        }
        s1 ++, s2 ++;
    }
    return 0;
  801a29:	b8 00 00 00 00       	mov    $0x0,%eax
}
  801a2e:	c9                   	leave  
  801a2f:	c3                   	ret    

00801a30 <main>:
#include <ulib.h>

/* try to load the kernel's TSS selector into the DS register */

int
main(void) {
  801a30:	55                   	push   %ebp
  801a31:	89 e5                	mov    %esp,%ebp
  801a33:	83 e4 f0             	and    $0xfffffff0,%esp
  801a36:	83 ec 10             	sub    $0x10,%esp
    asm volatile("movw $0x28,%ax; movw %ax,%ds");
  801a39:	66 b8 28 00          	mov    $0x28,%ax
  801a3d:	8e d8                	mov    %eax,%ds
    panic("FAIL: T.T\n");
  801a3f:	c7 44 24 08 e0 1e 80 	movl   $0x801ee0,0x8(%esp)
  801a46:	00 
  801a47:	c7 44 24 04 09 00 00 	movl   $0x9,0x4(%esp)
  801a4e:	00 
  801a4f:	c7 04 24 eb 1e 80 00 	movl   $0x801eeb,(%esp)
  801a56:	e8 eb e8 ff ff       	call   800346 <__panic>
