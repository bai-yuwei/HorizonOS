/******************************************************************************
* @file    Page_Table.h
* @brief   页表相关的头文件.
* @details This is the detail description.
* @author  ywBai <yw_bai@outlook.com>
* @date    2025年05月11日 (created)
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
* Date          : 2025年05月11日;
* Revision         : 0.0.1;
* Author           : ywBai;
* Contents         :
******************************************************************************/
#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include "Std_Types.h"
#include "Monitor.h"
#include "Interrupt.h"
#include "Bitmap.h"
#include "Math.h"

#define PAGE_SIZE  4096

// ********************* virtual memory layout *********************************
// 0xC0000000 ... 0xC0100000 ... 0xC0400000  boot & reserverd                4MB
// 0xC0400000 ... 0xC0800000 page tables, 0xC0701000 page directory          4MB
// 0xC0800000 ... 0xC0900000 kernel load                                     1MB
#define PAGE_DIR_VIRTUAL              0xC0701000
#define PAGE_TABLES_VIRTUAL           0xC0400000
#define KERNEL_LOAD_VIRTUAL_ADDR      0xC0800000
#define KERNEL_LOAD_PHYSICAL_ADDR     0x200000
#define KERNEL_SIZE_MAX               (1024 * 1024)

#define COPIED_PAGE_DIR_VADDR         0xFFFFE000
#define COPIED_PAGE_TABLE_VADDR       0xFFFFF000
#define COPIED_PAGE_VADDR             0xFFFFF000

// ********************* physical memory layout ********************************
// 0x00000000 ... 0x00100000  boot & reserved                                1MB
// 0x00100000 ... 0x00200000  kernel page tables                             1MB
// 0x00200000 ... 0x00300000  kernel load                                    1MB
// 0x01fff000 ... 0x01ffffff  kernel stack                                   4KB
#define KERNEL_PAGE_DIR_PHY           0x00101000

#define PHYSICAL_MEM_SIZE             (32 * 1024 * 1024)
#define KERNEL_BIN_LOAD_SIZE          (1024 * 1024)

/**
 * @struct page_table_entry
 * @brief 定义页表项的数据结构，用于描述虚拟地址到物理地址的映射信息。
 * 
 * 该结构体采用位域的方式，将 32 位的页表项拆分为不同的标志位和物理页框号。
 */
typedef struct page_table_entry
{
    /* 存在位，指示该页表项对应的物理页框是否存在于内存中。值为 1 表示物理页框存在于内存，值为 0 表示不在内存，访问时会触发页错误异常。*/
    uint32 present  : 1;
    /* 读写位，控制对该页表项对应物理页框的读写权限。值为 0 表示只读，值为 1 表示可读可写。
     */
    uint32 rw       : 1;
    /* 用户位，决定该页表项对应物理页框的访问权限级别。值为 0 表示只有内核模式可以访问，值为 1 表示用户模式和内核模式都可以访问。*/
    uint32 user     : 1;
    /* 访问位，记录该页表项对应物理页框是否被访问过。CPU 访问该页时会自动将此位置为 1，操作系统可定期清零该位来统计页面访问情况。*/
    uint32 accessed : 1;
    /* 脏位，指示该页表项对应物理页框的内容是否被修改过。CPU 对该页执行写操作时会自动将此位置为 1，用于页面置换时判断是否需要写回磁盘。*/
    uint32 dirty    : 1;
    /* 未使用位，目前在页表项中未被使用。*/
    uint32 unused   : 7;
    /* 物理页框号，存储该页表项对应物理页框的起始地址的高 20 位。与偏移量组合可得到完整的物理地址。*/
    uint32 frame    : 20;
} pte_t;

typedef struct page_directory_entry
{
    /* 存在位，指示该页表项对应的物理页框是否存在于内存中。值为 1 表示物理页框存在于内存，值为 0 表示不在内存，访问时会触发页错误异常。*/
    uint32 present  : 1;
    /* 读写位，控制对该页表项对应物理页框的读写权限。值为 0 表示只读，值为 1 表示可读可写。
     */
    uint32 rw       : 1;
    /* 用户位，决定该页表项对应物理页框的访问权限级别。值为 0 表示只有内核模式可以访问，值为 1 表示用户模式和内核模式都可以访问。*/
    uint32 user     : 1;
    /* 访问位，记录该页表项对应物理页框是否被访问过。CPU 访问该页时会自动将此位置为 1，操作系统可定期清零该位来统计页面访问情况。*/
    uint32 accessed : 1;
    /* 脏位，指示该页表项对应物理页框的内容是否被修改过。CPU 对该页执行写操作时会自动将此位置为 1，用于页面置换时判断是否需要写回磁盘。*/
    uint32 dirty    : 1;
    /* 未使用位，目前在页表项中未被使用。*/
    uint32 unused   : 7;
    /* 物理页框号，存储该页表项对应物理页框的起始地址的高 20 位。与偏移量组合可得到完整的物理地址。*/
    uint32 frame    : 20;
} pde_t;


typedef struct page_directory
{
    uint32 pdePhyAddress;
} page_directory_t;

void enable_Paging(void);
void reload_Page_Directory(page_directory_t *pageDirectory);
void page_Table_Init(void);
void page_Table_Test(void);

#endif // PAGE_TABLE_H