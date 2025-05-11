/******************************************************************************
* @file    Interrupt.h
* @brief   中断相关的头文件.
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

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "Std_Types.h"
#include "Io.h"
#include "Monitor.h"
#include "Stdlib.h"
#include "Gdt.h"

#define IDT_GATE_P     1
#define IDT_GATE_DPL0  0
#define IDT_GATE_DPL3  3
#define IDT_GATE_32_TYPE  0xE

#define IDT_GATE_ATTR_DPL0 \
  ((IDT_GATE_P << 7) + (IDT_GATE_DPL0 << 5) + IDT_GATE_32_TYPE)

#define IDT_GATE_ATTR_DPL3 \
  ((IDT_GATE_P << 7) + (IDT_GATE_DPL3 << 5) + IDT_GATE_32_TYPE)

#define IRQ0_INT_NUM 32
#define IRQ1_INT_NUM 33
#define IRQ2_INT_NUM 34
#define IRQ3_INT_NUM 35
#define IRQ4_INT_NUM 36
#define IRQ5_INT_NUM 37
#define IRQ6_INT_NUM 38
#define IRQ7_INT_NUM 39
#define IRQ8_INT_NUM 40
#define IRQ9_INT_NUM 41
#define IRQ10_INT_NUM 42
#define IRQ11_INT_NUM 43
#define IRQ12_INT_NUM 44
#define IRQ13_INT_NUM 45
#define IRQ14_INT_NUM 46
#define IRQ15_INT_NUM 47

#define SYSCALL_INT_NUM 0x80



struct idt_ptr
{
    uint16 limit;
    uint32 base;
} __attribute__((packed));
typedef struct idt_ptr idt_ptr_t;

/**
 * @struct idt_entry
 * @brief 中断描述符表（IDT）条目的结构体定义。
 * 
 * 该结构体用于表示 x86 架构下中断描述符表中的一个条目，
 * 每个条目对应一个中断或异常的处理信息。
 */
struct idt_entry
{
    /* 中断处理函数地址的低 16 位。与 handler_high 组合形成完整的 32 位中断处理函数地址。*/
    uint16 handler_low;

    /* 段选择子，指向中断处理函数所在的代码段。用于在全局描述符表（GDT）或局部描述符表（LDT）中选择代码段。*/
    uint16 selector;

    /* 保留字段，必须设置为 0。*/
    uint8  zero;

    /* 中断描述符的类型和属性。包含中断门、陷阱门等类型信息，以及描述符的特权级等属性。*/
    uint8  flags;

    /* 中断处理函数地址的高 16 位。与 handler_low 组合形成完整的 32 位中断处理函数地址。*/
    uint16 handler_high;
} __attribute__((packed));
typedef struct idt_entry idt_entry_t;


/**
 * @struct isr_params
 * @brief 中断服务例程（ISR）的参数结构体。
 * 
 * 该结构体用于保存中断发生时处理器的寄存器状态和相关信息，
 * 这些信息会在进入中断服务例程时被压入栈中，方便在 C 语言层面处理中断。
 */
typedef struct isr_params
{
    /* 数据段选择子。表示中断发生时使用的数据段。*/
    uint32 ds;

    /* 通用寄存器。依次为 EDI、ESI、EBP、ESP、EBX、EDX、ECX 和 EAX 寄存器的值，这些寄存器的值在中断发生时被保存。*/
    uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax;

    /* 中断号和错误码。intNum 表示触发中断的中断号，errCode 是某些中断会携带的错误码，若中断不产生错误码，该值通常为 0。*/
    uint32 intNum, errCode;
    /**
     * @brief 指令指针、代码段选择子、标志寄存器、用户栈指针和用户栈段选择子。
     * 
     * eip 是中断发生时即将执行的下一条指令的地址，
     * cs 是中断发生时使用的代码段选择子，
     * eflags 是标志寄存器的值，
     * userEsp 是用户模式下的栈指针，
     * ss 是用户模式下的栈段选择子。
     */
    uint32 eip, cs, eflags, userEsp, ss;
} isr_params_t;
/**
 * @brief 中断服务例程（ISR）的函数指针类型。
 *
 * 该类型定义了一个函数指针，指向一个接受 isr_params_t 类型参数的函数，
 * 该函数用于处理中断事件。
 */
typedef void (*isr_t)(isr_params_t);

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void isr32();
extern void isr33();
extern void isr34();
extern void isr35();
extern void isr36();
extern void isr37();
extern void isr38();
extern void isr39();
extern void isr40();
extern void isr41();
extern void isr42();
extern void isr43();
extern void isr44();
extern void isr45();
extern void isr46();
extern void isr47();


void enable_Interrupt(void);
void disable_Interrupt(void);
bool is_In_Interrupt(void);
void register_Interrupt_Handler(uint32 intNum, isr_t handler);
void idt_Init(void);
void isr_Handler(isr_params_t params);
#endif // INTERRUPT_H