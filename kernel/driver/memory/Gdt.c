/******************************************************************************
* @file    Gdt.c
* @brief   Gdt相关的文件.
* @details This is the detail description.
* @author  ywBai <yw_bai@outlook.com>
* @date    2025年05月06日 (created)
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
* Date          : 2025年05月06日;
* Revision         : 0.0.1;
* Author           : ywBai;
* Contents         :
******************************************************************************/

#include "Gdt.h"

extern void load_Gdt(gdt_ptr_t* gdt_ptr);
extern void refresh_Tss();

static gdt_ptr_t gdtPtr;
/* 
 * 定义了7个GDT段描述符
 * 分别是预留段、内核代码段、内核数据段、video段、用户代码段、用户数据段、TSS段
*/
static gdt_entry_t gdtEntries[7];
static tss_entry_t tssEntry;

/**
 * @brief 刷新全局描述符表（GDT）。
 * 
 * 该函数调用外部函数 `load_Gdt`，将预先定义好的 GDT 指针 `gdtPtr` 
 * 传递给 `load_Gdt` 函数，从而将 GDT 加载到 CPU 的 GDTR 寄存器中，
 * 完成 GDT 的刷新操作。
 */
static void refresh_Gdt()
{
    // 调用外部函数 load_Gdt，传入 GDT 指针，将 GDT 加载到 CPU 的 GDTR 寄存器
    load_Gdt(&gdtPtr);
}

/**
 * @brief 设置 GDT 表中指定索引位置的描述符。
 * 
 * 该函数根据传入的参数，对 GDT 表中指定索引的描述符进行初始化设置，
 * 包括段基地址、段界限、访问权限和粒度等信息。
 * 
 * @param num GDT 表中的索引位置，指定要设置的描述符条目。
 * @param base 段的基地址，一个 32 位的地址值。
 * @param limit 段的界限，定义了段的大小。
 * @param access 访问权限字节，包含段的类型、特权级等属性。
 * @param granularity 粒度字节，包含段界限的粒度和段大小等信息。
 */
static void set_Gdt(uint32 num, uint32 base, uint32 limit, uint8 access, uint8 granularity)
{
    // 设置段界限的低 16 位
    gdtEntries[num].limitLow = (limit & 0xFFFF);
    // 设置段基地址的低 16 位
    gdtEntries[num].baseLow = (base & 0xFFFF);
    // 设置段基地址的中间 8 位
    gdtEntries[num].baseMiddle = (base >> 16) & 0xFF;
    // 设置访问权限字节
    gdtEntries[num].access = access;
    // 设置粒度字节的低 4 位为段界限的高 4 位
    gdtEntries[num].granularity = (limit >> 16) & 0x0F;
    // 将传入的粒度字节的高 4 位合并到粒度字节中
    gdtEntries[num].granularity |= ((granularity << 4) & 0xF0);
    // 设置段基地址的高 8 位
    gdtEntries[num].baseHigh = (base >> 24) & 0xFF;
}

static void set_Tss(uint32 num, uint16 ss0, uint32 esp0)
{
    uint32 base = 0;
    uint32 limit = 0;
    memset(&tssEntry, 0, sizeof(tss_entry_t));
    tssEntry.ss0 = ss0;
    tssEntry.esp0 = esp0;
    tssEntry.io_map_base = sizeof(tss_entry_t);
    tssEntry.cs = SELECTOR_KERNEL_CODE | RPL3;
    tssEntry.ss = SELECTOR_KERNEL_DATA | RPL3;
    tssEntry.ds = SELECTOR_KERNEL_DATA | RPL3;
    tssEntry.es = SELECTOR_KERNEL_DATA | RPL3;
    tssEntry.fs = SELECTOR_KERNEL_DATA | RPL3;
    base = (uint32)&tssEntry;
    limit = base + sizeof(tssEntry);
    set_Gdt(num, base, limit, DESC_P | DESC_DPL_0 | DESC_S_SYS | DESC_TYPE_TSS, 0x0);
}

/**
 * @brief 初始化全局描述符表（GDT）和任务状态段（TSS）。
 * 
 * 此函数负责配置 GDT 指针，设置 GDT 表中的各个段描述符，
 * 包括预留段、内核代码段、内核数据段、video 段、用户代码段、用户数据段和 TSS 段，
 * 最后将 GDT 和 TSS 的设置加载到 CPU 中使其生效。
 */
void gdt_Init()
{
    // 计算 GDT 表的界限值，即 GDT 表的总字节数减 1。
    // 因为 GDT 表有 7 个描述符，每个描述符大小为 sizeof(gdt_entry_t)，界限值表示表的最大偏移量。
    gdtPtr.limit = sizeof(gdt_entry_t) * 7 - 1;
    // 设置 GDT 指针的基地址，指向 GDT 表在内存中的起始地址。
    gdtPtr.base = (uint32)&gdtEntries;

    // 设置 GDT 表的第 0 项为预留段，按照 x86 架构规范，GDT 的第 0 项必须为 0。
    set_Gdt(0, 0, 0, 0, 0);
    // 设置 GDT 表的第 1 项为内核代码段。
    // 基地址为 0，界限为 4GB，特权级为 0（内核级），表示代码段，粒度为 4KB 页粒度，32 位段。
    set_Gdt(1, 0, 0xFFFFF, DESC_P | DESC_DPL_0 | DESC_S_CODE | DESC_TYPE_CODE, FLAG_G_4K | FLAG_D_32);
    // 设置 GDT 表的第 2 项为内核数据段。
    // 基地址为 0，界限为 4GB，特权级为 0（内核级），表示数据段，粒度为 4KB 页粒度，32 位段。
    set_Gdt(2, 0, 0xFFFFF, DESC_P | DESC_DPL_0 | DESC_S_DATA | DESC_TYPE_DATA, FLAG_G_4K | FLAG_D_32);
    // 设置 GDT 表的第 3 项为 video 段。
    // 基地址为 0，界限为 7，特权级为 0（内核级），表示数据段，粒度为 4KB 页粒度，32 位段。
    set_Gdt(3, 0, 7,       DESC_P | DESC_DPL_0 | DESC_S_DATA | DESC_TYPE_DATA, FLAG_G_4K | FLAG_D_32);
    // 设置 GDT 表的第 4 项为用户代码段。
    // 基地址为 0，界限为 0xBFFFF，特权级为 3（用户级），表示代码段，粒度为 4KB 页粒度，32 位段。
    set_Gdt(4, 0, 0xBFFFF, DESC_P | DESC_DPL_3 | DESC_S_CODE | DESC_TYPE_CODE, FLAG_G_4K | FLAG_D_32);
    // 设置 GDT 表的第 5 项为用户数据段。
    // 基地址为 0，界限为 0xBFFFF，特权级为 3（用户级），表示数据段，粒度为 4KB 页粒度，32 位段。
    set_Gdt(5, 0, 0xBFFFF, DESC_P | DESC_DPL_3 | DESC_S_DATA | DESC_TYPE_DATA, FLAG_G_4K | FLAG_D_32);
    // 设置 GDT 表的第 6 项为 TSS 段。
    // 内核数据段选择子为 0x10，栈指针为 0。TSS 用于保存任务的上下文信息。
    set_Tss(6, 0x10, 0);

    // 调用 refresh_Gdt 函数，将配置好的 GDT 加载到 CPU 的 GDTR 寄存器中，使其生效。
    refresh_Gdt();
    // 调用 refresh_Tss 函数，刷新任务状态段，确保 TSS 信息正确加载到 CPU 中。
    refresh_Tss();
}

void updateTssEsp(uint32 esp)
{
    tssEntry.esp0 = esp;
}