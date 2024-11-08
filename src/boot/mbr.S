; /src/boot/mbr.S
; mbr的初始化
; mbr作用：将代码从磁盘加载到内存（初始化内存并搬运代码），跳转
%include "boot.inc"
; 定义MBR_BASE_ADDR，在src/boot/boot.inc中定义，地址的值为0x7C00。这个值由CPU确定，不能更改。
SECTION mbr vstart=MBR_BASE_ADDR

; mdr的入口标志
mbr_entry:
    ; 跳转到初始化内存的函数
    call init_segments
    ; 跳转到显示的函数
    call print_segments
    ; 跳转到装载loader的函数
    call load_loader_img
    ; 跳转到loader所在的地址
    jmp LOADER_BASE_ADDR

; 初始化内存的函数
init_segments:
    ; cs为代码段寄存器，将代码段的地址存入ax
    mov ax, cs
    ; 此时ax存放的是代码段的地址，将代码段的地址赋值给ds，ds是数据段寄存器，存放数据段地址
    mov ds, ax
    ; 将代码段的地址赋值给es，ds是额外数据段寄存器
    mov es, ax
    ; 将代码段的地址赋值给ss，ds是栈段寄存器
    mov ss, ax
    ; 将代码段的地址赋值给fs，fs是标志段寄存器，是80386起新增加的两个辅助段寄存器之一，指向当前活动线程的TEB结构
    ; 上述代码用cs寄存器的值初始化其他寄存器，此时cs寄存器的值为0xf000，所以上述代码实际上将ds、es、ss、fs均初始化为0xf000。
    mov fs, ax
    ; sp为栈段偏移地址，将栈偏移地址存入bx
    mov bx, sp
    ; 将栈段内容存入ax
    mov ax, [bx]
    ; 将数栈段内容存入栈段，偏移地址为“0x7b00”所在地址
    mov [0x7c00], ax
    ; 将0x7b00赋值给sp
    ; 上述代码为初始化栈指针，指定0x7c00以下的区域为栈区，并且复制原栈的内容
    mov sp, 0x7c00
    ret
    ; 回跳到call地址

print_segments:
    ; 0xb800为显存文本模式的内存地址。
    mov ax, 0xb800
    ; 将gs作为显存段地址
    mov gs, ax
    ; 利用0x06号功能，上卷全部行则可清屏
    ; 配置各寄存器
    ; AH 功能号 = 0x06
    ; AL 上卷的行数（为0则表示全部）
    ; BH 上卷的行属性
    ; (CL, CH) 窗口左上角的(X, Y)位置
    ; (DL, DH) 窗口右下角的(X, Y)位置
    mov ax, 0600h
    mov bx, 0700h
    ; 左上角(0, 0)位置
    mov cx, 0
    ; 右下角(80, 25)位置
    mov dx, 184fh
    ; VGA文本模式中，一行只能容纳80个字符，共25行
    int 10h
    ; 输出字符串：MBR
    ; A表示绿色背景闪烁，4表示前景色为红色
    mov byte [gs:0x00], '1'
    mov byte [gs:0x01], 0xA4
    mov byte [gs:0x02], ' '
    mov byte [gs:0x03], 0xA4
    mov byte [gs:0x04], 'M'
    mov byte [gs:0x05], 0xA4
    mov byte [gs:0x06], 'B'
    mov byte [gs:0x07], 0xA4
    mov byte [gs:0x08], 'R'
    mov byte [gs:0x09], 0xA4
    ret

; 装载loader的函数（读磁盘，存入内存）
load_loader_img:
    ; 读磁盘地址（loader镜像在磁盘的第二个sector，LOADER_START_SECTOR = 0x01），这里表示起始扇区地址。
    mov eax, LOADER_START_SECTOR
    ; 内存地址（loader加载地址为0x8000），即写入的地址。
    mov bx, LOADER_BASE_ADDR
    ; loader大小为8个sectors，共4KB，待读入的扇区数目。
    mov cx, 0x08
    ; 跳转到读磁盘的函数
    call read_disk
    ret

; 读磁盘的函数
read_disk:
    ; esi/edi为源/目标索引寄存器，用于存储指向源数据/目标数据的指针或索引。
    ; eax为累加器，是加法乘法指令的
    mov esi, eax
    ; di为目的变址寄存器
    mov di, cx
    ; 第一步，设置要读取的扇区数
    ; 设置要读取的磁盘扇区数目，在loader中已经将cx中存入磁盘数目
    mov dx, 0x01f2
    mov al,cl
    ; out为写入指令，将al中的值写入端口0x01f2，这个端口应该是硬盘所在的地址
    out dx, al
    ; 恢复eax
    mov eax, esi
    ; 第二步，将LBA地址存入0x1f3-0x1f6
    ; LBA指逻辑块地址（Logical Block Address，LBA），用于扇区的寻址
    mov dx, 0x1f3
    out dx, al    ;al已经存入磁盘数目
    ; LBA地址15-8位写入端口0x1f4
    ; shr表示逻辑右移（shift right，SHR），目的操作数中的每一位右移，原操作数决定移位的数目
    mov cl, 8
    shr eax, cl
    mov dx, 0x1f4
    out dx, al
    ; LBA地址23-16位写入端口0x1f5
    shr eax, cl
    mov dx, 0x1f5
    out dx, al
    ; LBA地址第24-27位
    shr eax, cl
    and al,0x0f
    ; 设置7-4位为1110，表示lba模式
    or al,0xe0
    mov dx, 0x1f6
    out dx, al
    ; 第三步，向0x1f7端口写入读命令，0x20
    mov dx, 0x1f7
    mov al, 0x20
    out dx, al
; 第四步，检测硬盘状态
.not_ready:
    nop
    ; in为输入指令，表示从dx地址端口读取数据到al，此时dx存放的是磁盘端口0x1f7
    in al, dx
    and al, 0x88
    ; cmp从目的操作数减去源操作数，不修改任何操作数，根据逻辑置位ZF和CF标志位
    ; jnz判断ZF标志位，若=0则跳转，否则不跳转
    ; 这里判断al是否等于0x08，不等于则循环该段
    cmp al, 0x08
    jnz .not_ready
    ; 第五步，从0x1f0读数据
    ; di为要读取的扇区数，一个扇区有512个字节，每次读入两个字节，所以di * 256
    mov ax, di
    mov dx, 256
    ; mul为乘操作，与ax寄存器相乘，结果在ax中
    mul dx
    mov cx, ax

    mov dx, 0x1f0

.go_on_read:
    in ax, dx
    mov [bx], ax
    add bx, 2
    loop .go_on_read
    ret

times 510 - ($ - $$) db 0
db 0x55, 0xaa