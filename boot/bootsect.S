!
! SYS_SIZE is the number of clicks (16 bytes) to be loaded.
! 0x3000 is 0x30000 bytes = 196kB, more than enough for current
! versions of linux
!
#include <linux/config.h>
SYSSIZE = 0x3000 

.globl begtext, begdata, begbss, endtext, enddata, endbss
.text
begtext:
.data
begdata:
.bss
begbss:
.text

SETUPLEN = 4				! the len of setup = 4
BOOTSEG  = 0x07c0			! bootsect.S被导入的地方
INITSEG  = 0x9000			! 0x9000
SETUPSEG = 0x9020			! 0x9200
SYSSEG   = 0x1000			! system 装载在 0x10000 (65536).
ENDSEG   = SYSSEG + SYSSIZE		! system代码段的结束地址


ROOT_DEV = 0			
! 设备号 = 主设备号*256 + 次设备号
! 1-内存， 2-磁盘， 3-硬盘， 4-ttyx, 5-tty, 6-并行口, 7-非命名管道

SWAP_DEV = 0

entry start					！告诉链接程序这里开始
start:
	mov	ax,#BOOTSEG			! 段寄存器ds, es 设置成#BOOTSEG
	mov	ds,ax				
	mov	ax,#INITSEG			! #INITSEG地址是0x9000
	mov	es,ax
	mov	cx,#256				! cx 段长256
	sub	si,si				！si, di 清零, es:di -> ds:si
	sub	di,di
	rep						! #循环
	movw
	jmpi	go,INITSEG		! 到go地址处继续执行代码, 分别是段内偏移地址和段地址

go:	mov	ax,cs				
	mov	dx,#0xff00	! arbitrary value >>512 - disk parm size

	mov	ds,ax	
	mov	es,ax
	push	ax

	mov	ss,ax		! put stack at 0x9ff00 - 12.
	mov	sp,dx
/*
 *	Many BIOS's default disk parameter tables will not 
 *	recognize multi-sector reads beyond the maximum sector number
 *	specified in the default diskette parameter tables - this may
 *	mean 7 sectors in some cases.
 *
 *	Since single sector reads are slow and out of the question,
 *	we must take care of this by creating new parameter tables
 *	(for the first disk) in RAM.  We will set the maximum sector
 *	count to 18 - the most we will encounter on an HD 1.44.  
 *
 *	High doesn't hurt.  Low does.
 *
 *	Segments are as follows: ds=es=ss=cs - INITSEG,
 *		fs = 0, gs = parameter table segment
 */


	push	#0
	pop	fs
	mov	bx,#0x78		! fs:bx is parameter table address
	seg fs
	lgs	si,(bx)			! gs:si is source

	mov	di,dx			! es:di is destination
	mov	cx,#6			! copy 12 bytes
	cld

	rep
	seg gs
	movw

	mov	di,dx
	movb	4(di),*18		! patch sector count

	seg fs
	mov	(bx),di
	seg fs
	mov	2(bx),es

	pop	ax
	mov	fs,ax
	mov	gs,ax
	
	xor	ah,ah			! reset FDC 
	xor	dl,dl
	int 	0x13	

! load the setup-sectors directly after the bootblock.
! Note that 'es' is already set up.

/*
	利用BIOS中断INT 0x13将setup模块从磁盘第二个扇区读到0x90200处，共读4个扇区，
	这里导入的程序就是setup.s,如果读出错，那么就要进行恢复，把硬盘驱动器复位，并且重试
	读扇区
	ah = 0x02 - 读磁盘扇区到内存   al = 需要读出来的扇区数量
	ch = 磁道号第8位,		cl = 开始扇区 + 磁道号高2位
	dh = 磁头号				dl = 驱动器号
	es:bx->指向数据缓冲区
 */
load_setup:
	xor	dx, dx			! drive 0, head 0
	mov	cx,#0x0002		! sector 2, track 0
	mov	bx,#0x0200		! address = 512, in INITSEG
	mov	ax,#0x0200+SETUPLEN	! service 2, nr of sectors
	int	0x13			! read it
	jnc	ok_load_setup		! ok - continue

	mov dx, #0x0000
	mov ax, #0x0000
	push	ax			! dump error code
	call	print_nl
	mov	bp, sp
	call	print_hex
	pop	ax	
	
	xor	dl, dl			! reset FDC
	xor	ah, ah


	int	0x13
	j	load_setup		! jmp -> load_setup

ok_load_setup:

! Get disk drive parameters, specifically nr of sectors/track

	mov dl,#0x00
	mov	ax,#0x0800		! AH=8 is get drive parameters
	int	0x13
	mov ch,#0x00
	xor	ch,ch
	seg cs
	mov	sectors,cx
	mov	ax,#INITSEG
	mov	es,ax			! 上面去磁盘参数中断修改了es的值，这里需要改回去

! Print some inane message
! 显示信息 "Welcome to LiNos System\n"  

/*
	BIOS int0x10 ah=0x03, read cursor pos
	input: bh = page number
	output: ch = scan start line,  cl=scan end line, dh = row number, dl = column number

	BIOS int0x13 ah=0x13, print string
	input: al= the way to set cursor. 0x01使用bl中的属性值, 光标停在字符串结尾处
	es:bp 此寄存器对指向要显示的字符串起始位置处, cx=显示的字符串字符数， bh=显示页面号
	bl = 字符属性， dh= row number, dl= column number
 */

	mov	ah,#0x03		! read cursor pos
	xor	bh,bh
	int	0x10
	
	mov	cx,#26
	mov	bx,#0x0007		! page 0, attribute 7 (normal)
	mov	bp,#msg1
	mov	ax,#0x1301		! write string, move cursor
	int	0x10

! ok, we've written the message, now
! we want to load the system (at 0x10000)

	mov	ax,#SYSSEG
	mov	es,ax		! segment of 0x010000
	call	read_it		! read system module on the disk, es as an input parameter
	call	kill_motor	! shutdown the motor of the disk driver, then we can get the status of the driver
	call	print_nl	! print the status of the driver


/*
	然后检查哪个根文件系统设备被使用了，如果已经指定了设备，就直接使用给定的设备，否则根据BIOS报告的每磁道扇区数
	来确定到底使用/dev/PS0 (2,28) 还是/dev/ato (2,8)
 */
	seg cs
	mov	ax,root_dev
	or	ax,ax
	jne	root_defined
	seg cs
	mov	bx,sectors
	mov	ax,#0x0208		! /dev/ps0 - 1.2Mb
	cmp	bx,#15			! 判断每磁道的扇区数是否是15
	je	root_defined	! 如果是15, ax中就是引导驱动器的设备号
	mov	ax,#0x021c		! /dev/PS0 - 1.44Mb
	cmp	bx,#18
	je	root_defined
undef_root:
	jmp undef_root
root_defined:
	seg cs
	mov	root_dev,ax


! 跳转到setup.s运行, 代码块就在bootsect.s后面
	jmpi	0,SETUPSEG

! This routine loads the system at address 0x10000, making sure
! no 64kB boundaries are crossed. We try to load it as fast as
! possible, loading whole tracks whenever we can.
!
! in:	es - starting address segment (normally 0x1000)
!
sread:	.word 1+SETUPLEN	! sectors read of current track当前磁道中已读扇区数
head:	.word 0			! current head 当前磁头号
track:	.word 0			! current track	当前的磁道号

/*
	首先测试输入的段值，必须是位于内存地址64KB的边界开始处, 否则进入死循环。 清bx,用于表示当前内存放数据的开始位置
	表示从段地址偏移量=0的位置开始
 */

read_it:
	mov ax,es
	test ax,#0x0fff		! ax = 0x1000, ZF = 0
die:	jne die			! es must be at 64kB boundary
	xor bx,bx		! bx is starting address within segment

! 是否已经加载了全部的数据?
! 看是否已经处于数据所在的最后一段,末尾段？
rp_read:
	mov ax,es
	cmp ax,#ENDSEG		! have we loaded all yet?
	jb ok1_read
	ret


/*
	计算当前磁道所需要读取的扇区数，放在ax寄存器里面
	根据当前磁道还没有读取的扇区数以及段内数据字节开始偏移位置，计算如果读取完这些数据，是否会越界
	64KB的限制，如果会超过，就根据这次最多能读入的字节数，计算出能读的扇区数
 */
ok1_read:
	seg cs
	mov ax,sectors
	sub ax,sread
	mov cx,ax
	shl cx,#9
	add cx,bx
	jnc ok2_read
	je ok2_read
	xor ax,ax
	sub ax,bx
	shr ax,#9


ok2_read:
	call read_track
	mov cx,ax
	add ax,sread
	seg cs
	cmp ax,sectors
	jne ok3_read
	mov ax,#1
	sub ax,head
	jne ok4_read
	inc track
ok4_read:
	mov head,ax
	xor ax,ax
ok3_read:
	mov sread,ax
	shl cx,#9
	add bx,cx
	jnc rp_read
	mov ax,es
	add ah,#0x10
	mov es,ax
	xor bx,bx
	jmp rp_read

！ 读取当前磁道上指定开始扇区和虚读扇区数的值到es:bx中，al 虚读扇区数，es:bx 缓冲区开始位置

read_track:
	pusha
	pusha
	mov	ax, #0xe2e 	! loading... message 2e = .
	mov	bx, #7
 	int	0x10
	popa		

	mov dx,track			! 取当前磁道号
	mov cx,sread			！取当前磁道上已读扇区数
	inc cx					！开始读扇区数
	mov ch,dl				！ch=当前磁道号
	mov dx,head
	mov dh,dl
	mov dl,#0				! A驱动器
	and dx,#0x0100
	mov ah,#2
	
	push	dx				! save for error dump
	push	cx
	push	bx
	push	ax

	int 0x13
	jc bad_rt
	add	sp, #8   	
	popa
	ret

! #出错则进行复位操作，ax=0,再跳转到read_track再试
bad_rt:	
	push	ax				! save error code
	call	print_all			! ah = error, al = read
	
	xor ah,ah
	xor dl,dl
	int 0x13
	
	add	sp, #10
	popa	
	jmp read_track

/*
 *	print_all is for debugging purposes.  
 *	It will print out all of the registers.  The assumption is that this is
 *	called from a routine, with a stack frame like
 *	dx 
 *	cx
 *	bx
 *	ax
 *	error
 *	ret <- sp
 *
*/
 
print_all:
	mov	cx, #5		! error code + 4 registers
	mov	bp, sp	

print_loop:
	push	cx		! save count left
	call	print_nl	! nl for readability
	jae	no_reg		! see if register name is needed
	
	mov	ax, #0xe05 + 0x41 - 1
	sub	al, cl
	int	0x10

	mov	al, #0x58 	! X
	int	0x10

	mov	al, #0x3a 	! :
	int	0x10

no_reg:
	add	bp, #2		! next register
	call	print_hex	! print it
	pop	cx
	loop	print_loop
	ret

print_nl:
	mov	ax, #0xe0d	! CR
	int	0x10
	mov	al, #0xa	! LF
	int 	0x10
	ret

/*
 *	print_hex is for debugging purposes, and prints the word
 *	pointed to by ss:bp in hexadecmial.
*/

print_hex:
	mov	cx, #4		! 4 hex digits
	mov	dx, (bp)	! load word into dx
print_digit:
	rol	dx, #4		! rotate so that lowest 4 bits are used
	mov	ah, #0xe	
	mov	al, dl		! mask off so we have only next nibble
	and	al, #0xf
	add	al, #0x30	! convert to 0 based digit, '0'
	cmp	al, #0x39	! check for overflow
	jbe	good_digit
	add	al, #0x41 - 0x30 - 0xa 	! 'A' - '0' - 0xa

good_digit:
	int	0x10
	loop	print_digit
	ret


/*
 * This procedure turns off the floppy drive motor, so
 * that we enter the kernel in a known state, and
 * don't have to worry about it later.
 * 0x3f2是软盘控制器的一个端口，被称为数字输出寄存器DOR端口，是一个8位寄存器，7-4用于控制四个软盘驱动器，
 * 3-2用于允许或者禁止DMA和中断请求以及启动复位控制器FDC，位1-位0用于选择操作的软驱，第236行上al中设置输出的0值
 * 选择A驱动器，关闭FDC，禁止DMA和中断请求，关闭马达
 */

kill_motor:
	push dx
	mov dx,#0x3f2		！软驱控制卡的数字输出寄存器DOR端口，只能写
	xor al, al			！关闭FDC，禁止DMA和中断请求，关闭马达
	outb				！将al中的内容输出到dx指定的端口去
	pop dx
	ret

sectors:
	.word 0				！存放当前启动软盘每磁道的扇区数

msg1:
	.byte 13,10			！回车和换行的ASCII码
	.ascii "Welcome to LiNos System..."

.org 506
swap_dev:
	.word SWAP_DEV
root_dev:
	.word ROOT_DEV
boot_flag:
	.word 0xAA55

.text
endtext:
.data
enddata:
.bss
endbss:

