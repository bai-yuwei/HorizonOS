%include "boot.inc"

SECTION loader vstart=LOADER_BASE_ADDR

loader_entry:
    jmp loader_start

;***************************** data section  **********************************;
;************ 显示内容定义 ***************;
; 定义显示的行和列
cursor_row: dd 0
cursor_col: dd 0
; 定义加载保护模式时的显示内容
setup_protection_mode_message:
    db "setup protection mode ... "

setup_protection_mode_message_length equ $ - setup_protection_mode_message
; 定义初始化页表时的显示内容
setup_page_message:
    db "setup page ... "
    db 0

; 定义初始化内核时的显示内容
init_kernel_message:
    db "init kernel ... "
    db 0
; 加载内核 panic 时的显示内容
panic_message:
    db "PANIC!"
    db 0
;************ GDT ***************;
; GDT的基址定义
GDT_BASE:           dd 0x00000000
                    dd 0x00000000
; 代码段描述符
CODE_DESC:          dd DESC_CODE_LOW_32
                    dd DESC_CODE_HIGH_32
; 数据段描述符
DATA_STACK_DESC:    dd DESC_DATA_LOW_32
                    dd DESC_DATA_HIGH_32
; VIDEO段描述符
VIDEO_DESC:         dd DESC_VIDEO_LOW_32   ;dd 0x80000007
                    dd DESC_VIDEO_HIGH_32
; 预留60个描述符的空位
times 60 dq 0
; 计算GDT大小
GDT_SIZE equ $ - GDT_BASE
; 计算段界限
GDT_LIMIT equ GDT_SIZE - 1
; 选择子定义
SELECTOR_CODE    equ (0x0001 << 3) + TI_GDT + RPL0
SELECTOR_DATA    equ (0x0002 << 3) + TI_GDT + RPL0
SELECTOR_VIDEO   equ (0x0003 << 3) + TI_GDT + RPL0

; 定义
gdt_ptr dw GDT_LIMIT
        dd GDT_BASE

;***************************** code section  **********************************;

loader_start:
    call clean_screen
    call setup_protection_mode
    jmp $

; 清屏函数，不赘述了，调用0x10中断清屏，和 mbr.S 所用的方式相同
clean_screen:
    mov byte ah, 0x06
    mov byte al, 0x00
    mov byte bh, 0x07
    ; start (0, 0)
    mov byte cl, 0x00
    mov byte ch, 0x00
    ; end: (dl, dh) = (x:79, y:24)
    mov byte dl, 0x4f
    mov byte dh, 0x18
    int 0x10
    ret

; 进入保护模式时显示函数，向显存存入内容即可
; 这实际上是一个函数，只需要向 ax 存入要显示的内容，向 cx 存入要显示内容的长度,然后调用即可
print_message_real_mode:
    mov bp, ax
    mov ah, 0x13  ; int num
    mov al, 0x01
    mov bh, 0x00  ; page number
    mov bl, 0x07
    int 0x10
    ret

; 进入保护模式
setup_protection_mode:
    mov ax, setup_protection_mode_message
    mov dx, 0x00
    mov cx, setup_protection_mode_message_length
    call print_message_real_mode
    ; 打开A20
    in al, 0x92
    or al, 0000_0010b
    out 0x92, al
    ; 加载gdt
    lgdt [gdt_ptr]
    ; cr0第0位置1
    mov eax, cr0
    or eax, 0x00000001
    mov cr0, eax
    ; 通过远跳刷新流水线
    jmp SELECTOR_CODE:p_mode_start
; 进入保护模式之后的任务
[bits 32]
p_mode_start:
    ; 设置数据段
    mov ax, SELECTOR_DATA
    mov ds, ax
    mov es, ax
    mov ss, ax
    ; 设置显示段
    mov ax, SELECTOR_VIDEO
    mov gs, ax

    ; 设置光标
    push setup_protection_mode_message_length
    call move_cursor
    add esp, 4

    ; 输出 OK 表示上面设置完成
    call print_OK
    ; 打印输出换行
    call print_new_line

    ; 输出正在初始化页表信息
    push setup_page_message
    call print_message
    add esp, 4
    ; 创建页表
    call setup_page
    call print_OK
    call print_new_line

    ; 输出正在加载内核信息
    push init_kernel_message
    call print_message
    add esp, 4
    ; 加载内核
    call init_kernel

    ; 不应该运行到这里
    push panic_message
    call print_message
    add esp, 4

    jmp $
    ret

; 打印信息的函数
print_message:
    ; 保存上一个函数栈的基址，更新本函数的基址
    push ebp
    mov ebp, esp
    ; 保存 esi
    push esi
    mov esi, [ebp + 8]   ; arg: message ptr

    call get_current_cursor_addr
    mov ecx, eax
    mov eax, 0
    .print_byte:
        mov byte dl, [esi + eax]
        cmp dl, 0
        je  .print_end
        mov byte [gs:ecx], dl
        mov byte [gs:ecx + 1], 0x7  ; white on black
        add eax, 1
        add ecx, 2
        jmp .print_byte

    .print_end:
        push eax
        call move_cursor
        add esp, 4

        pop esi
        pop ebp
        ret

; 打印 OK 字符
print_OK:
    ; let's test video memory - a green [OK] after message1
    call get_current_cursor_addr

    mov byte [gs:(eax+0)], '['
    mov byte [gs:(eax+2)], 'O'
    mov byte [gs:(eax+3)], 0010b
    mov byte [gs:(eax+4)], 'K'
    mov byte [gs:(eax+5)], 0010b
    mov byte [gs:(eax+6)], ']'

    push 4
    call move_cursor
    add esp, 4

    ret
; 打印输出换行
print_new_line:
    mov eax, [cursor_row]
    add eax, 1
    mov dword [cursor_row], eax
    mov dword [cursor_col], 0
    push 0
    call move_cursor
    add esp, 4
    ret

; 获取当前光标地址
get_current_cursor_addr:
    mov ecx, [cursor_row]
    mov edx, [cursor_col]

    mov eax, ecx
    shl eax, 6
    shl ecx, 4
    add eax, ecx  ; ecx * 64 + ecx * 16 = row * 80
    add eax, edx
    shl eax, 1

    ret

; 32位保护模式移动光标，直接使用 in 和 out 指令访问存放光标位置的端口
move_cursor:
    ; 保存上一个函数栈的基址，更新本函数的基址
    push ebp
    mov ebp, esp

    ; 计算光标要移动的位置，传入的数据存储在栈中
    mov eax, [ebp + 8]
    mov ecx, [cursor_row]
    mov edx, [cursor_col]
    add edx, eax

    ; 判断是否需要到下一行，一行最多80字符
    .start_new_row:
        cmp edx, 80
        jle .do_move
        inc ecx
        sub edx, 80
        jmp .start_new_row

    ; 移动光标
    .do_move:
        mov dword [cursor_row], ecx
        mov dword [cursor_col], edx

        call get_current_cursor_addr
        shr eax, 1
        mov ecx, eax

        mov al, 14
        mov dx, 0x3D4
        out dx, al
        mov al, ch
        mov dx, 0x3D5
        out dx, al

        mov al, 15
        mov dx, 0x3D4
        out dx, al
        mov al, cl
        mov dx, 0x3D5
        out dx, al

        pop ebp
        ret

;************ 创建页表 ***************;
; 所谓页表，实际上就是一段连续的内存空间，里面连续存放 pde/pte
setup_page:
    push PAGE_SIZE
    push PAGE_DIR_TABLE_ADDR
    call clear_memory
    add esp, 8
    ; 创建 pde
    .create_pde:
        ; 在内存中，每一个 pde/pte 为 4B 。物理空间共 4G ，每一个 pte 指向 4K 的物理地址，因此需要 1M 个 pte 。
        ; 这 1M 个 pte 可以直接放在一张页表中，或者拆分为 1K 个页表，每个页表存放 1K 个页表项。
        ; 每个 pde 指向一个页表，因此使用二级页表的话共有 1K 个 pde。
        ; 在内存中，页目录表和页表的布局是直接连续存放的，先存放页目录表，再存放页表（页目录表为低地址，页表为高地址）。
        ; 此时 eax 存放的是页目录表的物理地址
        mov eax, PAGE_DIR_PHYISCAL_ADDR
        or eax, PG_US_U | PG_RW_W | PG_P
        ; 将第一个 pde 存放在页目录表的首地址
        mov [PAGE_DIR_TABLE_ADDR + 0], eax
        ; 将页目录表的第 0 项和第 768 项都指向同一个页表，这是因为操作系统会放在 3GB-4GB 的虚拟空间内。
        mov [PAGE_DIR_TABLE_ADDR + 768 * 4], eax
        ; 最后一个页目录项指向页目录表自己的地址
        sub eax, 0x1000
        mov [PAGE_DIR_TABLE_ADDR + 4092], eax

        ; 创建操作系统的其他 pde （虚拟地址中的 3GB-4GB 的虚拟空间）
        mov eax, PAGE_DIR_TABLE_ADDR + PAGE_SIZE
        or eax, PG_US_U | PG_RW_W | PG_P

        ; ecx 中存储了循环次数（第一个地址已经写入值了）
        mov eax, PAGE_DIR_PHYISCAL_ADDR
        add eax, 0x2000
        mov ecx, 254
        mov edx, PAGE_DIR_TABLE_ADDR + 769 * 4
        ; 创建内核（高1GB）的页目录表
        .create_kernel_pde:
            mov [edx], eax
            add eax, PAGE_SIZE
            add edx, 4
            loop .create_kernel_pde
    ; 创建页表
    mov eax, 0
    or eax, PG_US_U | PG_RW_W | PG_P
    mov ecx, 256
    mov edx, PAGE_DIR_PHYISCAL_ADDR
    .create_pte:
        mov [edx], eax
        add eax, PAGE_SIZE
        add edx, 4
        loop .create_pte
    ret

; 使能页表，就是将描述符的地址和偏移量写入内存 gdt_ptr，然后用新地址（虚拟地址）重新加载
; 说人话，就是给原来的 gdt 搬个家，从物理地址搬到新的物理地址（高 1GB 的空间），然后通过页表去读写
enable_page:
    ; 存储带原来 gdt 所在的位置
    sgdt [gdt_ptr]
    ; 改变 gdt 描述符中视频段描述符的位置
    mov ebx, [gdt_ptr + 2]
    or dword [ebx + 0x18 + 4], 0xC0000000

    ; move gdt to > 0xC0000000
    add dword [gdt_ptr + 2], 0xC0000000

    ; move stack to 0xC0000000
    mov eax, [esp]
    add esp, 0xc0000000
    mov [esp], eax

    ; 把页目录的地址给 cr3，这是建立页表后的第二步
    mov eax, PAGE_DIR_TABLE_ADDR
    mov cr3, eax

    ; enable paging on cr0 register，这是建立页表后的第三步
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    ; load gdt again - gdt has been moved to > 0xC0000000
    lgdt [gdt_ptr]

    ; refresh video segment selector cache
    mov ax, SELECTOR_VIDEO
    mov gs, ax

    ret

; 为虚拟内存->物理内存设置连续内核页面映射的函数
;  - arg1：虚拟地址开始（页面对齐）
;  - arg2：物理地址开始（页面对齐）
;  - arg3：页数
;
; 调用者必须确保从arg2开始的物理内存可以自由使用
set_pages_mapping:
    ; 保存环境
    push ebp
    mov ebp, esp
    push esi
    push edi
    push ebx

    ; 虚拟起始地址
    mov esi, [ebp + 8]
    shr esi, 12
    ; 物理起始地址
    mov edi, [ebp + 12]
    ; 需要的页数
    mov ecx, [ebp + 16]
    ; 计算页表索引，实际上就是除以 4KB
    ; 计算起始 pte
    or edi, PG_US_U | PG_RW_W | PG_P
    .map_next_page:
        ; 通过虚拟地址访问页表
        mov [PAGE_VIRTUAL_ADDR_START + esi * 4], edi
        add esi, 1
        add edi, PAGE_SIZE
        loop .map_next_page
    pop ebx
    pop edi
    pop esi
    pop ebp
    ret

; 内存空间清理函数
clear_memory:
    push ebp
    mov ebp, esp

    mov eax, [ebp + 8]   ; start addr
    mov ecx, [ebp + 12]  ; size
    .clear_byte:
        mov byte [eax], 0
        add eax, 1
        loop .clear_byte

    pop ebp
    ret

;****************************** init kernel ***********************************;
init_kernel:
    ; 从内存中分配空间，存放 kernel.bin、kernel 和 kernel 运行需要的空间
    call allocate_pages_for_kernel
    ; 从硬盘中读 kernel.bin 并存入内存
    call load_hd_kernel_image
    ; 初始化 kernel
    call do_load_kernel
    ; 初始化浮点数寄存器
    finit
    ; move stack to 0xF0000000
    mov esp, KERNEL_STACK_TOP - 16
    mov ebp, esp

    ; let's jump to kernel entry :)
    jmp eax
    ret


; 由于已经建立页表，因此需要申请内存用于存放 kernel 源文件
allocate_pages_for_kernel:
    ; 申请存放 kernel.bin 的空间
    mov ecx, KERNEL_BIN_MAX_SIZE
    shr ecx, 12
    ; arg3：此时 ecx 中存放 kernel 的 bin 源文件需要的页数
    push ecx
    ; 存储 kernel.bin 存放的实际物理地址
    push KERNEL_BIN_LOAD_PHYSICAL_ADDR
    ; 存储 kernel.bin 存放的虚拟地址
    push KERNEL_BIN_LOAD_VIRTUAL_ADDR
    call set_pages_mapping
    add esp, 12

    ; 申请存放 kernel 的空间
    mov ecx, KERNEL_SIZE_MAX
    shr ecx, 12
    push ecx  ; arg 3: num of pages to place kernel
    push KERNEL_PHYSICAL_ADDR_START  ; arg 2: physical addr
    push KERNEL_VIRTUAL_ADDR_START   ; arg 1: virtual addr
    call set_pages_mapping
    add esp, 12

    ; 申请 kernel 工作的栈空间
    mov ecx, 2
    push ecx  ; arg 3: num of pages
    push KERNEL_STACK_PHYSICAL_ADDR        ; arg 2: physical addr
    push KERNEL_STACK_TOP - PAGE_SIZE      ; arg 1: virtual addr
    call set_pages_mapping
    add esp, 12

    ret

load_hd_kernel_image:
    mov eax, KERNEL_START_SECTOR
    mov ebx, KERNEL_BIN_LOAD_VIRTUAL_ADDR
    mov ecx, KERNEL_SECTORS
    call read_disk_32
    ret

; 初始化 kernel
; 初始化内核实际上就是将 kernel.bin 中的 segment 拷贝到各段自己被编译的虚拟地址处。
do_load_kernel:
    ; 清零寄存器
    xor eax, eax
    ; ebx 记录程序头表地址
    xor ebx, ebx
    ; cx 记录程序头表中 program header 数量
    xor ecx, ecx
    ; dx 记录 program header 尺寸，即 e_phentsize
    xor edx, edx
    ; 偏移文件 42 字节处的属性是 e_phentsize，表示 program header 大小
    mov dx, [KERNEL_BIN_LOAD_VIRTUAL_ADDR + 42]   ; e_phentsize
    ; 偏移文件开始部分 28 字节是 e_phoff
    mov ebx, [KERNEL_BIN_LOAD_VIRTUAL_ADDR + 28]  ; e_phoff
    add ebx, KERNEL_BIN_LOAD_VIRTUAL_ADDR
    ; 偏移文件开始部分 44 字节是 e_phnum，表示有几个 program header
    mov cx, [KERNEL_BIN_LOAD_VIRTUAL_ADDR + 44]   ; e_phnum
    .load_each_segment:
        ; 若 p_type 和 0 相等，表示此 program header 未使用
        cmp byte [ebx + 0], 0
        je .next_prgoram_header
        ; 为函数 memcpy 压入参数
        push dword [ebx + 16]  ; 压入 memcpy 第三个参数，size
        mov eax, [ebx + 4]     ; p_offset
        add eax, KERNEL_BIN_LOAD_VIRTUAL_ADDR
        push eax               ; 压入 memcpy 第二个参数，源地址
        push dword [ebx + 8]   ; 压入 memcpy 第一个参数，目的地址
        ; 调用 memcpy 完成段复制
        call memcpy
        ; 清理压入栈的三个参数
        add esp, 12
    .next_prgoram_header:
        ; edx 为 program header 大小
        add ebx, edx
        loop .load_each_segment
    ; return the kernel entry address
    mov eax, [KERNEL_BIN_LOAD_VIRTUAL_ADDR + 24]
    ret

memcpy:
    cld
    push ebp
    mov ebp, esp
    push esi
    push edi
    push ecx

    mov edi, [ebp + 8]    ; dst
    mov esi, [ebp + 12]   ; src
    mov ecx, [ebp + 16]   ; size
    rep movsb
    call print_OK
    pop ecx
    pop edi
    pop esi
    pop ebp
    ret

; 读硬盘，和 mbr 一致，唯一区别已经建立了页表，只需要向虚拟地址写
read_disk_32:
    mov esi, eax
    mov edi, ecx

    ; sector count
    mov dx, 0x01f2
    mov al, cl
    out dx, al

    mov eax, esi

    ; LBA low
    mov dx, 0x1f3
    out dx, al

    ; LBA mid
    shr eax, 8
    mov dx, 0x1f4
    out dx, al

    ; LBA high
    shr eax, 8
    mov dx, 0x1f5
    out dx, al

    ; device reg: LBA[24:28]
    shr eax, 8
    and al, 0x0f

    or al, 0xe0  ; 0x1110, LBA mode
    mov dx, 0x1f6
    out dx, al

    ; command reg: 0x2 read, start reading
    mov dx, 0x1f7
    mov al, 0x20
    out dx, al

    .hd_not_ready:
        nop
        in al, dx
        and al, 0x88  ; bit 7 (busy), bit 3 (data ready)
        cmp al, 0x08
        jnz .hd_not_ready

    ; di = cx = sector count
    ; read 2 bytes time, so loop (sector count) * 512 / 2 times
    mov eax, edi
    mov edx, 256
    mul edx
    mov ecx, eax

    mov dx, 0x1f0

    .go_on_read_data:
        in ax, dx
        mov [ebx], ax
        add ebx, 2
        loop .go_on_read_data

    ret
