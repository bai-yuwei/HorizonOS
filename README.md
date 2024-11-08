# HorizonOS开发文档
- [HorizonOS开发文档](#horizonos开发文档)
  - [项目说明](#项目说明)
  - [开发环境](#开发环境)
    - [Ubuntu安装](#ubuntu安装)
    - [GCC编译器安装](#gcc编译器安装)
    - [NASM编译器安装](#nasm编译器安装)
    - [bochs安装和配置](#bochs安装和配置)
      - [bochs安装](#bochs安装)
      - [bochs配置](#bochs配置)
    - [配置SFTP](#配置sftp)
  - [操作系统的架构和工作流程](#操作系统的架构和工作流程)
    - [bootloader](#bootloader)
    - [MBR](#mbr)
      - [初始化内存](#初始化内存)
      - [读硬盘数据](#读硬盘数据)
      - [操作显卡显示](#操作显卡显示)

## 项目说明
<p>本项目从零开始构建一个操作系统。
<p>本文档详细说明开发思路和具体操作。

## 开发环境
1. Ubuntu + GCC编译器 + NASM编译器 + bochs
2. Windows10 + VS Code
### Ubuntu安装
见 https://blog.csdn.net/Python_0011/article/details/131633534
### GCC编译器安装
在Linux终端执行sudo apt install build-essential
### NASM编译器安装
在Linux终端执行sudo apt-get install nasm
### bochs安装和配置
#### bochs安装
在Linux终端执行sudo apt install bochs
#### bochs配置
1. 创建bochsrc.txt

内容如下：
````
# RAM size
megs: 32
# Change to your Bochs installation path
romimage: file=/usr/share/bochs/BIOS-bochs-latest
vgaromimage: file=/usr/share/bochs/VGABIOS-lgpl-latest

# Disk
boot: disk
ata0: enabled=1, ioaddr1=0x01f0, ioaddr2=0x03f0, irq=14
ata0-master: type=disk, path="scroll.img", mode=flat, cylinders=6, heads=16, spt=63

log: bochsout.txt

mouse: enabled=0
keyboard_mapping: enabled=1, map=/usr/share/bochs/keymaps/x11-pc-us.map
clock: sync=realtime
cpu: ips=1000000
````
其中，``romimage: file=/usr/share/bochs/BIOS-bochs-latest
vgaromimage: file=/usr/share/bochs/VGABIOS-lgpl-latest``根据自己bochs的安装位置进行修改，``path="scroll.img``就是镜像文件，可自行命名，创建的镜像文件需要和``bochsrc.txt``在同一路径，其他参数可不修改。

2. 创建配置文件

应用命令行工具``bximage``构建配置文件。命令如下：
````
>> bximage
========================================================================
                                bximage
                  Disk Image Creation Tool for Bochs
          $Id: bximage.c 11315 2012-08-05 18:13:38Z vruppert $
========================================================================

Do you want to create a floppy disk image or a hard disk image?
Please type hd or fd. [hd] hd

What kind of image should I create?
Please type flat, sparse or growing. [flat] flat

Enter the hard disk size in megabytes, between 1 and 8257535
[10] 1

I will create a 'flat' hard disk image with
  cyl=2
  heads=16
  sectors per track=63
  total sectors=2016
  total size=0.98 megabytes

What should I name the image?
[c.img] test

Writing: [] Done.

I wrote 1032192 bytes to test.

The following line should appear in your bochsrc:
  ata0-master: type=disk, path="test", mode=flat, cylinders=2, heads=16, spt=63
````
其中一般需要修改``Enter the hard disk size in megabytes, between 1 and 8257535``后的大小，这是要构建的硬盘大小。文件名称需要与第一步构建的``bochsrc.txt``中的文件名称一致。

3. 运行Bochs进行测试

输入命令
````
bochs -f bochsrc.txt
````
![alt text](image.png)
<p>进入该界面表示默认进入调试模式，输入c即可正常运行。弹窗提示``No bootable device``为正常现象，表示配置成功。

### 配置SFTP
见 https://blog.csdn.net/iamlujingtao/article/details/102501845
## 操作系统的架构和工作流程
### bootloader
<p>操作系统在工作时，首先需要将操作系统本身载入内存，这一般通过一段额外的引导程序来完成，这段引导程序就是bootloader。在硬件电路上电的初期，可以运行的只有已经被固定存放在某一个存储单元中的程序，这段程序在CPU上电时被默认搬运到某个地址，然后运行，这由程序运行依赖的硬件载体决定。这段程序就是bootloader，作用就是初始化内核运行所必须的硬件外设，例如内存，flash，以太网等，然后把内核搬运到内存中。

<p>对于X86系列的CPU而言，在初始上电时可以访问1MB的内存空间，这1MB的空间就是所谓的实模式下的内存，如下图所示。至于为什么实模式只有1MB，这和X86的地址总线宽度有关。事实上，在这1MB中，低640KB地址指向的是DRAM，高64KB地址指向的是ROM，至于为什么这么指，和实际芯片引脚连接的设备有关。事实上，这只是对于8086而言，对于其他芯片，可能会有其他的定义，但不同的芯片地址空间不同，操作这些芯片的方法都是一致的。

![alt text](image-4.png)
### MBR
<p>如果有过单片机的基础，实际上bootloader就是一段单片机裸机的代码。对于X86而言，采用了两级boot的形式，即先运行一个更简单的bootloader来驱动一个复杂的bootloader，再由更复杂的bootloader来驱动内核，这样的boot方式在嵌入式系统或者计算机中都是常见的，因为可以极大减少始终存放在flash或者rom中的计算机程序，把bootloader的一大部分也可以存放在硬盘中。一般会把最初始的那一段bootloader称为一级boot，然后依次类推二级boot，三级boot等。对于X86系统，MBR就是他的一级boot的别称。
<p>对于X86系统，规定了MBR的大小必须是512字节，CPU规定了MBR运行的起始地址为0x7C00。MBR的作用是初始化内存，将二级boot从硬盘读入内存，然后跳转到二级boot。为了方便调试，我们可以在MBR中加入操作显卡的代码，从而通过屏幕打印调试信息来判断程序是否正确执行。至于MBR如何被搬入内存，这是由计算机主板的BIOS程序完成的。BIOS在自检通过后，将MBR扇区整个读到内存中，然后跳转执行MBR。所以如果把BIOS也看做boot的一部分，那么MBR是二级boot，后面的bootloader是三级boot。

#### 初始化内存
<p>汇编语言允许我们直接操作相关寄存器。以32位CPU为例，包括段寄存器、数据寄存器和变址寄存器等。CS为代码段寄存器，在CPU工作时会将内存分为代码段、数据段、堆栈段等，段寄存器中存放了相关的段基址。在MBR代码中，此时的段地址是0xf000，这个值就是CPU上电时BIOS代码所在的段地址。一个段内包含的地址空间为0x0000-0xffff。

````
mov ax, cs
mov ds, ax
mov es, ax
mov ss, ax
mov fs, ax
````
以上代码很好理解，就是读当前的段地址，然后分别定义数据段、堆栈段的段地址。MBR程序只能运行在初始的1MB空间内，因此段地址都是0xf000。之后是定义偏移地址
````
mov bx, sp
mov ax, [bx]
mov [0x7c00], ax
````
这是在保存现场，我们希望初始化内存到偏移地址为0x7c00的位置处，因此把当前程序运行地址内的数据先搬移到0x7c00地址处。
````
mov sp, 0x7c00
````
这里直接赋值sp指针，将当前偏移地址设置为0x7c00。所以初始化内存，就是设置程序运行的段地址和偏移地址。
#### 读硬盘数据
汇编语言可以实现对硬盘数据的读写，具体步骤如下：
1. 备份eax和cx数据
````
mov esi,eax
mov di,cx
````
在读硬盘数据之前，我们将待读硬盘的起始扇区所在地址存入eax，将待读扇区数目存入cx，然后分别复制给esi和di做备份。
2. 设置要读取的扇区数
````
mov dx, 0x01f2
mov al,cl
; out为写入指令，将al中的值写入端口0x01f2，这个端口应该是硬盘所在的地址
out dx, al
; 恢复eax
mov eax, esi
````
0x01f2是硬盘控制器的端口寄存器地址
3. 将LBA地址存入0x1f3-0x1f6
LBA就是硬盘的逻辑块地址
````
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
````
操作端口寄存器需要用out指令，具体参考下面的文章https://blog.csdn.net/fjlq1994/article/details/49472827
操作端口寄存器用out指令。这里其实就是把一个很长的LBA地址分不同位写到端口寄存器里面，移位写入的循环。
4. 向0x1f7端口写入读命令，0x20
````
mov dx, 0x1f7
mov al, 0x20
out dx, al
````
5. 检测硬盘状态
````
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
````
这里有个循环，in指令读端口寄存器，判断标志位，否则循环。
6. 从0x1f0读数据
````
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
````
cx是循环次数，循环读256次读取一个扇区，总循环次数为256 * di。
#### 操作显卡显示
![alt text](image-3.png)