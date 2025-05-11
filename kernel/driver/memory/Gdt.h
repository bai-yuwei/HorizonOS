/******************************************************************************
* @file    Gdt.h
* @brief   Gdt相关的头文件.
* @details This is the detail description.
* @author  ywBai <yw_bai@outlook.com>
* @date    2025年04月27日 (created)
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

#ifndef GDT_H
#define GDT_H

#include "Std_Types.h"
#include "Stdlib.h"

#define FLAG_G_4K  (1 << 3)
#define FLAG_D_32  (1 << 2)

#define DESC_P     (1 << 7)

#define DESC_DPL_0   (0 << 5)
#define DESC_DPL_1   (1 << 5)
#define DESC_DPL_2   (2 << 5)
#define DESC_DPL_3   (3 << 5)

#define DESC_S_CODE   (1 << 4)
#define DESC_S_DATA   (1 << 4)
#define DESC_S_SYS    (0 << 4)

#define DESC_TYPE_CODE  0x8   // r/x non-conforming code segment
#define DESC_TYPE_DATA  0x2   // r/w data segment
#define DESC_TYPE_TSS   0x9

#define RPL0 0
#define RPL1 1
#define RPL2 2
#define RPL3 3

#define TI_GDT 0
#define TI_LDT 1

#define SELECTOR_KERNEL_CODE    ((1 << 3) | (TI_GDT << 2) | RPL0)
#define SELECTOR_KERNEL_DATA    ((2 << 3) | (TI_GDT << 2) | RPL0)
#define SELECTOR_KERNEL_STACK   ((2 << 3) | (TI_GDT << 2) | RPL0)
#define SELECTOR_VIDEO          ((3 << 3) | (TI_GDT << 2) | RPL0)
#define SELECTOR_USER_CODE      ((4 << 3) | (TI_GDT << 2) | RPL3)
#define SELECTOR_USER_DATA      ((5 << 3) | (TI_GDT << 2) | RPL3)


/*
 * GDT Pointer
 * 描述一个GDT表的指针结构体
 * 包含GDT表的界限和基地址
*/
struct gdt_ptr
{
    uint16 limit;
    uint32 base;
} __attribute__((packed));
typedef struct gdt_ptr gdt_ptr_t;
/*
 * GDT Entry
 * 描述一个GDT表项的结构体
 * 包含段界限、段基地址、访问标志和存储粒度信息
 * 用于描述一个段的属性和权限
 * 每个表项占用8个字节
 * 访问标志和存储粒度信息的具体含义需要根据具体的CPU架构和指令集来确定
 */
struct gdt_entry
{
    uint16 limitLow;       /* Limit (0-15) 存储段界限的低16位 */
    uint16 baseLow;        /* Base (0-15) 存储段基地址的低16位*/
    uint8 baseMiddle;     /* Base (16-23) 存储段基地址的中间8位*/
    uint8 access;           /* Access 访问标志 */
    uint8 granularity;      /* Granularity 存储粒度字节*/
    uint8 baseHigh;        /* Base (16-23) 存储段基地址的高6位*/
} __attribute__((packed));  /* 告诉编译器不要对结构体进行字节对齐 */
typedef struct gdt_entry gdt_entry_t;

struct tss_entry
{
    uint32 prev_tss;
    uint32 esp0;
    uint32 ss0;
    uint32 esp1;
    uint32 ss1;
    uint32 esp2;
    uint32 ss2;
    uint32 cr3;
    uint32 eip;
    uint32 eflags;
    uint32 eax;
    uint32 ecx;
    uint32 edx;
    uint32 ebx;
    uint32 esp;
    uint32 ebp;
    uint32 esi;
    uint32 edi;
    uint32 es;
    uint32 cs;
    uint32 ss;
    uint32 ds;
    uint32 fs;
    uint32 gs;
    uint32 ldt;
    uint16 trap;
    uint16 io_map_base;
} __attribute__((packed));
typedef struct tss_entry tss_entry_t;


void gdt_Init();

#endif