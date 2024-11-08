%include "boot.inc"
section loader vstart=LOADER_BASE_ADDR
LOADER_STACK_TOP equ LOADER_BASE_ADDR

jmp loader_start

GDT_BASE:           dd 0x00000000
                    dd 0x00000000
; 代码段描述符
CODE_DESC:          dd DESC_CODE_LOW_32
                    dd DESC_CODE_HIGH_32
; 数据段描述符
DATA_STACK_DESC:    dd DESC_DATA_LOW_32
                    dd DESC_DATA_HIGH_32
; VIDEO段描述符
VIDEO_DESC:         dd 0x80000007
                    dd DESC_VIDEO_HIGH_32
; 计算GDT大小
GDT_SIZE equ $ - GDT_BASE
; 计算段界限
GDT_LIMIT equ GDT_SIZE - 1
; 预留60个描述符的空位
; times 40 dq 0

SELECTOR_CODE    equ (0x0001 << 3) + TI_GDT + RPL0
SELECTOR_DATA    equ (0x0002 << 3) + TI_GDT + RPL0
SELECTOR_VIDEO   equ (0x0003 << 3) + TI_GDT + RPL0


; 定义
gdt_ptr dw GDT_LIMIT
        dd GDT_BASE

loadermsg db '2 loader in real. '
loader_start:
    ; 初始化寄存器
    ; 初始化栈地址为LOADER_BASE_ADDR，则初始化sp指针为LOADER_BASE_ADDR指向栈首
    mov sp, LOADER_BASE_ADDR
    ; es：bp为字符串地址，打印使用
    mov bp, loadermsg
    ; cx为字符串长度，后面循环使用
    mov cx, 17
    mov ax, cs
    mov es, ax
    ; 定义显示地址
    mov ax, 0x1301
    ; 页号和颜色，显示使用
    mov bx, 0x001f
    mov dx, 0x1800
    ; 10h号中断
    int 0x10
    jmp $