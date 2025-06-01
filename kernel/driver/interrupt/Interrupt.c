/******************************************************************************
* @file    Interrupt.c
* @brief   中断相关的文件.
* @details This is the detail description.
* @author  ywBai <yw_bai@outlook.com>
* @date    2025年05月10日 (created)
* @version 0.0.1
* @par Copyright (C):
*          Bai, yuwei. All Rights Reserved.
* @par Encoding:
*          UTF-8
* @par Description        :
* 1. Hardware Descriptions:
*      None.
* 2. Program Architecture:
*      None.
* 3. File Usage:
*      None.
* 4. Limitations:
*      None.
* 5. Else:
*      None.
* @par Modification:
* Date          : 2025年05月10日;
* Revision         : 0.0.1;
* Author           : ywBai;
* Contents         :
******************************************************************************/
#include "Interrupt.h"

extern void reload_Idt(uint32 idtPtrAddress);

idt_ptr_t idtPtr;
static idt_entry_t idt[256];
static isr_t interruptHandlers[256];
bool inIrqFLag = false;

static void set_Idt_Entry(uint8 num, uint32 base, uint16 selector, uint8 flags)
{
    idt[num].handler_low = base & 0xFFFF;
    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].flags = flags;
    idt[num].handler_high = (base >> 16) & 0xFFFF;
}

/**
 * @brief 初始化 8259A 可编程中断控制器（PIC）。
 * 
 * 该函数用于对主从 8259A PIC 进行初始化配置，包括设置初始化命令字（ICW），
 * 并解除所有中断请求（IRQ）的屏蔽，使得 PIC 可以响应外部中断。
 */
static void pic_Init()
{
    // 初始化主 8259A PIC
    // 发送 ICW1：设置初始化命令字 1，启动初始化过程，边沿触发，级联模式
    io_Out_Byte(0x20, 0x11);
    // 发送 ICW2：设置主 PIC 中断向量起始地址为 0x20
    io_Out_Byte(0x21, 0x20);
    // 发送 ICW3：设置主 PIC 级联引脚，表明从 PIC 连接在主 PIC 的 IRQ2 引脚上
    io_Out_Byte(0x21, 0x04);
    // 发送 ICW4：设置 8086/8088 模式，非自动结束中断（EOI）
    io_Out_Byte(0x21, 0x01);
    
    // 初始化从 8259A PIC
    // 发送 ICW1：设置初始化命令字 1，启动初始化过程，边沿触发，级联模式
    io_Out_Byte(0xA0, 0x11);
    // 发送 ICW2：设置从 PIC 中断向量起始地址为 0x28
    io_Out_Byte(0xA1, 0x28);
    // 发送 ICW3：设置从 PIC 级联识别码，表明从 PIC 连接在主 PIC 的 IRQ2 引脚上
    io_Out_Byte(0xA1, 0x02);
    // 发送 ICW4：设置 8086/8088 模式，非自动结束中断（EOI）
    io_Out_Byte(0xA1, 0x01);
    
    // 解除所有中断请求（IRQ）的屏蔽
    // 向主 PIC 的数据端口（0x21）写入 0x0，解除主 PIC 所有 IRQ 的屏蔽
    io_Out_Byte(0x21, 0x0);
    // 向从 PIC 的数据端口（0xA1）写入 0x0，解除从 PIC 所有 IRQ 的屏蔽
    io_Out_Byte(0xA1, 0x0);
}

/**
 * @brief 启用全局中断。
 * 
 * 该函数通过内联汇编指令启用 CPU 的中断功能，使得 CPU 能够响应外部中断请求。
 * 在调用此函数后，CPU 会开始处理接收到的中断信号。
 */
void enable_Interrupt(void)
{
    // 使用内联汇编执行 "sti" 指令，sti 是 "Set Interrupt Flag" 的缩写，
    // 该指令会将 CPU 的中断标志位（IF）置为 1，从而启用全局中断。
    // __volatile__ 关键字告诉编译器不要对该汇编代码进行优化，确保指令按原样执行。
    __asm__ __volatile__("sti");
}

/**
 * @brief 禁用全局中断。
 * 
 * 该函数通过内联汇编指令禁用 CPU 的中断功能，使得 CPU 暂时无法响应外部中断请求。
 * 在调用此函数后，CPU 会忽略接收到的中断信号，直到再次启用中断。
 */
void disable_Interrupt(void)
{
    // 使用内联汇编执行 "cli" 指令，cli 是 "Clear Interrupt Flag" 的缩写，
    // 该指令会将 CPU 的中断标志位（IF）置为 0，从而禁用全局中断。
    // __volatile__ 关键字告诉编译器不要对该汇编代码进行优化，确保指令按原样执行。
    __asm__ __volatile__("cli");
}

bool is_In_Interrupt(void)
{
    return inIrqFLag;
}

void register_Interrupt_Handler(uint32 intNum, isr_t handler)
{
    interruptHandlers[intNum] = handler;
}

/**
 * @brief 初始化中断描述符表（IDT）。
 * 
 * 该函数负责初始化中断描述符表，设置中断描述符表指针，
 * 清空中断描述符表和中断处理程序数组，
 * 并为每个中断向量设置对应的中断服务例程入口，最后重新加载 IDT。
 */
void idt_Init(void)
{
    // 初始化中断描述符表（IDT）

    // 设置中断描述符表指针的界限，界限值为 IDT 总大小减 1
    // IDT 总大小为每个条目大小（idt_entry_t）乘以 256 个条目
    idtPtr.limit = sizeof(idt_entry_t) * 256 - 1;
    // 设置中断描述符表指针的基地址，指向 IDT 数组的起始地址
    idtPtr.base = (uint32)&idt;

    // 将 IDT 数组的所有内容清零，确保初始状态为空
    memset(&idt, 0, sizeof(idt_entry_t) * 256);
    // 将中断处理程序数组的所有内容清零，确保初始状态没有注册的处理程序
    memset(&interruptHandlers, 0, sizeof(isr_t) * 256);

    // 为每个中断向量设置对应的中断服务例程入口
    // 中断向量号从 0 到 47，将每个中断向量与对应的中断服务例程关联
    // isr0 - all 函数在汇编文件中创建
    // 异常
    set_Idt_Entry(0, (uint32)isr0, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(1, (uint32)isr1, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(2, (uint32)isr2, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(3, (uint32)isr3, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(4, (uint32)isr4, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(5, (uint32)isr5, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(6, (uint32)isr6, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(7, (uint32)isr7, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(8, (uint32)isr8, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(9, (uint32)isr9, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(10, (uint32)isr10, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(11, (uint32)isr11, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(12, (uint32)isr12, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(13, (uint32)isr13, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(14, (uint32)isr14, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(15, (uint32)isr15, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(16, (uint32)isr16, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(17, (uint32)isr17, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(18, (uint32)isr18, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(19, (uint32)isr19, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(20, (uint32)isr20, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(21, (uint32)isr21, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(22, (uint32)isr22, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(23, (uint32)isr23, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(24, (uint32)isr24, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(25, (uint32)isr25, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(26, (uint32)isr26, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(27, (uint32)isr27, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(28, (uint32)isr28, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(29, (uint32)isr29, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(30, (uint32)isr30, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(31, (uint32)isr31, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    // 中断
    set_Idt_Entry(32, (uint32)isr32, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(33, (uint32)isr33, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(34, (uint32)isr34, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(35, (uint32)isr35, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(36, (uint32)isr36, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(37, (uint32)isr37, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(38, (uint32)isr38, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(39, (uint32)isr39, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(40, (uint32)isr40, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(41, (uint32)isr41, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(42, (uint32)isr42, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(43, (uint32)isr43, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(44, (uint32)isr44, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(45, (uint32)isr45, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(46, (uint32)isr46, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    set_Idt_Entry(47, (uint32)isr47, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);
    // 预留系统调用中断向量，当前注释掉，后续可根据需要启用
    // set_Idt_Entry(SYSCALL_INT_NUM, (uint32)isr_48, SELECTOR_KERNEL_CODE, IDT_GATE_ATTR_DPL3);

    // 重新加载中断描述符表，使新的 IDT 设置生效
    reload_Idt((uint32)&idtPtr);
    // 初始化可编程中断控制器（PIC）
    pic_Init();
}

/**
 * @brief 中断服务例程的处理函数。
 * 
 * 该函数用于处理中断事件，根据中断号执行相应的操作，
 * 特别是对硬件中断进行确认，以允许后续中断继续触发。
 * 
 * @param params 包含中断发生时处理器状态和相关信息的结构体。
 */
void isr_Handler(isr_params_t params)
{
    // 从传入的参数结构体中获取中断号
    uint32 intNum = params.intNum;
    isr_t handler;
    // X86中，0-31号中断是由CPU硬件产生的，称之为异常。
    // 32-47号中断是由外部设备产生的，称之为（硬）中断。
    // 48-255号中断是由软件产生的，称之为软件中断。
    // 检查中断号是否在硬件中断范围（32 到 SYSCALL_INT_NUM 之间）
    if (intNum >= 32 && intNum <= 47)
    {
        // 检查中断号是否大于等于 40，大于等于 40 的中断由从片 8259A 管理
        if (intNum >= 40)
        {
            // 向从片 8259A 的命令端口（0xA0）发送 EOI（End of Interrupt）命令（0x20），
            // 通知从片 8259A 中断处理已经完成，允许继续响应后续中断
            io_Out_Byte(0xA0, 0x20);
        }
        // 向主片 8259A 的命令端口（0x20）发送 EOI 命令，
        // 通知主片 8259A 中断处理已经完成，允许继续响应后续中断
        io_Out_Byte(0x20, 0x20);
        inIrqFLag = true;
    }
    else
    {
        // 仅在处理硬件中断时关闭中断，处理异常和软件中断均开启中断
        enable_Interrupt();
    }
    if (interruptHandlers[intNum] != 0)
    {
        // 如果存在中断处理程序，则调用相应的中断处理程序
        handler = interruptHandlers[intNum];
        handler(params);
    }
    else
    {
        // 如果没有中断处理程序，则打印错误信息
        monitor_Printf("Error: unknown interrupt num: %d\n", intNum);
    }

    if (inIrqFLag)
    {
        inIrqFLag = false;
        enable_Interrupt();
    }
}