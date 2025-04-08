; macros definition
;********************************** MBR ***************************************;
MBR_BASE_ADDR          equ  0x7C00
;********************************* loader *************************************;
LOADER_BASE_ADDR       equ  0x900
LOADER_START_SECTOR    equ  2

;********************************** gdt ***************************************;
; G属性：记录段界限的粒度，这里定义段界限的粒度为4KB。位于第55位，在高32位的第24位，因此值为1 << 23。
DESC_G_4K  equ  1 << 23
; D/B属性：意义随着段描述符是代码段、向下扩展数据段和栈段而不同。
; 代码段（S = 1，TYPE = *1**），该位称作D属性，设置该属性则被视为32位代码段，否则为16位代码段。
; 栈段，该位称作B属性，设置该属性则在堆栈访问指令中采用32位堆栈指针寄存器（ESP），否则采用16位堆栈指针寄存器（SP）。
; 向下扩展数据段，设置该属性则段的上届为4GB，否则为64KB
; 位于第54位，在高32位的第22位，因此值为1 << 22。
DESC_D_32  equ  1 << 22
; L属性：该属性仅在IA-32e模式下有意义，标记该段是否为64位代码段。
; L = 1，则该段为64位代码段。若L = 1，则必须设置D = 0。
; L在第53位，高32位的21位。
DESC_L_32  equ  0 << 21
; AVL属性：由操作系统、应用程序自行定义。AVL在第52位，高32位的第20位。
DESC_AVL   equ  0 << 20
; P属性：标记段描述符是否有效。P = 1为有效（在内存中存在），P = 0为无效。
; 位于第47位，高32位的第15位。
DESC_P      equ  1 << 15
; DPL属性：记录了访问段所需要的特权级，共四级（0-3），越小特权级越高。
; 位于第45-46位，即高32位的第13和14位。
DESC_DPL_0  equ  00b << 13
DESC_DPL_1  equ  01b << 13
DESC_DPL_2  equ  10b << 13
DESC_DPL_3  equ  11b << 13
; S属性：描述了描述符的类型，S = 0表示该描述符对应的段是系统段，S = 1 表示描述的段时数据段或代码段。
; 位于第44位，即高32位的第12位。
DESC_S_CODE  equ  1 << 12
DESC_S_DATA  equ  1 << 12
DESC_S_SYS   equ  0 << 12
; TYPE属性：描述存储段的类型信息，意义随S属性不同而不同。
; 位于第40-43位，高32位的第8-11位。
; S = 1，第0位（第40位）为0表示该段为数据段
;   第1位（第41位）：描述段的增长方向，为0为向上增长，为1为向下增长。
;   第2位（第42位）：描述段的写权限，为0为只读，为1为可读可写
;   第3位（第43位）：描述段的访问标记，为0表示该段未被访问过，为1表示该段已被访问过
; S = 1，第0位（第40位）为1表示该段为代码段
;   第1位（第41位）：描述一致代码段标记，为0表示不是一致代码段，为1表示是一致代码段。
;       一致代码段指该段可以被低特权的用户直接访问，不能被高特权用户直接访问。非一致代码段指该段代码仅能被同特权级用户访问。
;   第2位（第42位）：描述段的读写权限，为0为只读，为1为可读可写
;   第3位（第43位）：描述段的访问标记，为0表示该段未被访问过，为1表示该段已被访问过
; S = 0（该段为系统段时）过多状态，自行百度。
DESC_TYPE_CODE equ 1000b << 8    ; 该段为代码段，非一致代码段，只读，未被访问
DESC_TYPE_DATA equ 0010b << 8    ; 该段为数据段，非一致代码段，可读可写，未被访问

; code segment
; 描述段界限，0-15位和48-51位，即低32位的0-15位和高32位的16-19位。
; 采取平坦模型，即处理器按照段基址+段偏移生成物理地址。
; 定义段基址为0，段界限*粒度=4G，粒度选择4K，则段界限为0XFFFFF。
DESC_LIMIT_CODE_0_15    equ   0xFFFF
DESC_LIMIT_CODE_16_19   equ   0xF << 16
; 定义代码段描述符的高32位，段基址为全0
DESC_CODE_HIGH_32       equ     (0x00 << 24) + \
                                DESC_G_4K + \
                                DESC_D_32 + \
                                DESC_L_32 + \
                                DESC_AVL + \
                                DESC_LIMIT_CODE_16_19 + \
                                DESC_P + \
                                DESC_DPL_0 + \
                                DESC_S_CODE + \
                                DESC_TYPE_CODE + \
                                0x00
; 定义代码段描述符的低32位，段基址为全0
DESC_CODE_LOW_32        equ   (0x00 << 16) + DESC_LIMIT_CODE_0_15

; data segment，同上，不再介绍
DESC_LIMIT_DATA_0_15    equ   0xFFFF
DESC_LIMIT_DATA_16_19   equ   0xF << 16

DESC_DATA_HIGH_32       equ     (0x00 << 24) + \
                                DESC_G_4K + \
                                DESC_D_32 + \
                                DESC_L_32 + \
                                DESC_AVL + \
                                DESC_LIMIT_DATA_16_19 + \
                                DESC_P + \
                                DESC_DPL_0 + \
                                DESC_S_DATA + \
                                DESC_TYPE_DATA + \
                                0x00

DESC_DATA_LOW_32        equ   (0x00 << 16) + DESC_LIMIT_DATA_0_15

; video segment
; 存放video的段，实际上也是数据段，同上，段大小不同
DESC_LIMIT_VIDEO_0_15   equ   0x0007  ; video has 8 pages
DESC_LIMIT_VIDEO_16_19  equ   0x0 << 16

DESC_VIDEO_HIGH_32      equ     (0x00 << 24) + \
                                DESC_G_4K + \
                                DESC_D_32 + \
                                DESC_L_32 + \
                                DESC_AVL + \
                                DESC_LIMIT_VIDEO_16_19 + \
                                DESC_P + \
                                DESC_DPL_0 + \
                                DESC_S_DATA + \
                                DESC_TYPE_DATA + \
                                0x0B

DESC_VIDEO_LOW_32       equ   (0x8 << 28) + DESC_LIMIT_VIDEO_0_15


;*************************** segment selector *********************************;
; RPL：请求特权级别，对应DPL属性
RPL0 equ 00b
RPL1 equ 01b
RPL2 equ 10b
RPL3 equ 11b
; TI = 0时查GDT表，TI = 1时查LDT表
TI_GDT equ 000b
TI_LDT equ 100b
; 剩余空间为Index索引值，处理器将索引值*8 + GDT或LDT基地址就是要加载的段描述符

;********************************* page ***************************************;
; 0x100000为页目录表的物理地址，这是出了低1MB空间的第一个字节。
PAGE_DIR_TABLE_ADDR               equ   0x100000
; 页目录大小（共4KB，每一个PTE为32位4B表示一个页表，一个页目录包含1024个PTE）
PAGE_SIZE                         equ   0x1000
; 这表示第一个页表的物理地址（手动地让页表和页目录表在物理地址中相连）
PAGE_DIR_PHYISCAL_ADDR            equ   PAGE_DIR_TABLE_ADDR + PAGE_SIZE
; 页表虚拟地址，开启页表后编辑页表通过此虚拟地址访问
PAGE_VIRTUAL_ADDR_START           equ   0xFFc00000
PAGE_VIRTUAL_TABLE_ADDR_START     equ   0xFFFFF000
; 页目录项 PDE 和页表项 PTE 的属性
; 页表为N行1列的表格，PTE是页表中的一行，作用是存储内存物理地址。
; 页目录项存储每个页表的物理地址。
; 页目录项和页表项均为4字节大小（32位），其中31-12位为其存储的物理页地址，其他位表示属性
; P位为存在位，是PDE和PTE的第0位，1表示该页存在于物理内存中，0表示该表不存在于物理内存中。
PG_P     equ  1
; R/W位为读写标志，是PDE和PTE的第1位，为1表示可读可写，为0表示可读不可写。
PG_RW_R  equ  0 << 1
PG_RW_W  equ  1 << 1
; US位用户标志位，是PDE和PTE的第2位，为1表示为普通用户，允许任意级别程序访问该页，为0表示为特权用户，仅允许特权级别为0、1、2的程序访问该页。
PG_US_S  equ  0 << 2
PG_US_U  equ  1 << 2
; 其余位均置0
;******************************** kernel **************************************;
; 页目录表和页表的实际存储空间（物理地址空间的范围）为 0x100000 - 0x102000，共 8KB
; 选择将 kernel.bin 存放在 0x0102000 - 0x2000000 之间（这个可以自定义，约 32 MB）
PHY_MEM_SIZE  equ  32 * 1024 * 1024

; kernel.bin 在硬盘的第 9 个扇区开始存储
KERNEL_START_SECTOR     equ   9
; kernel.bin 最大 1MB
KERNEL_BIN_MAX_SIZE     equ   1024 * 1024  ; 1MB
KERNEL_SECTORS          equ   KERNEL_BIN_MAX_SIZE / 512

; kernel 在虚拟地址中存放在高 1GB 空间，因此地址最大为 0xFFc00000（最后一个页表项用来存页表的地址了）
KERNEL_BIN_LOAD_MEM_MAX         equ   0xFF000000
; 虚拟地址自上而下存储
KERNEL_BIN_LOAD_VIRTUAL_ADDR    equ   KERNEL_BIN_LOAD_MEM_MAX - KERNEL_BIN_MAX_SIZE + 1;
; 物理地址自上而下存储
; 减去 PAGE_SIZE 是为了确保可以存储的下 kernel，因为由于页表的限制，物理内存单元最小读取大小为 4KB，其实就是页面对齐。
KERNEL_BIN_LOAD_PHYSICAL_ADDR   equ   PHY_MEM_SIZE - PAGE_SIZE - KERNEL_BIN_MAX_SIZE
; 存放处理后的 kernel 地址空间
KERNEL_VIRTUAL_ADDR_START       equ   0xC0800000
KERNEL_PHYSICAL_ADDR_START      equ   0x200000  ; 2MB
KERNEL_SIZE_MAX                 equ   1024 * 1024
; kernel 运行所需栈空间
KERNEL_STACK_TOP                equ   0x9F000
KERNEL_STACK_PHYSICAL_ADDR      equ   PHY_MEM_SIZE - PAGE_SIZE
