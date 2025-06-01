/******************************************************************************
* @file    Kheap.h

* @brief   内核堆管理相关的头文件.
* @details This is the detail description.
* @author  ywBai <yw_bai@outlook.com>
/**
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
 * Date          : 2025年05月17日;
 * Revision         : 0.0.1;
 * Author           : ywBai;
 * Contents         : 初始创建文件
 ******************************************************************************/

#ifndef KHEAP_H
#define KHEAP_H

#include "Std_Types.h"
#include "Monitor.h"
#include "Ordered_Array.h"
#include "Page_Table.h"

#define HEADER_SIZE (sizeof(kheap_block_header_t))
#define FOOTER_SIZE (sizeof(kheap_block_footer_t))
#define BLOCK_META_SIZE (sizeof(kheap_block_header_t) + sizeof(kheap_block_footer_t))

#define PAGE_ALIGNED        1
#define NOT_PAGE_ALIGNED    0

#define IS_FREE   1
#define NOT_FREE  0

#define KHEAP_START          0xC0C00000
#define KHEAP_MIN_SIZE       0x300000
#define KHEAP_MAX            0xE0000000

#define KHEAP_INDEX_NUM      0x20000
#define KHEAP_MAGIC          0x123060AB

struct kheap_block_header
{
    uint32 magic;
    bool isFree;
    uint32 size;
} __attribute__((packed));
typedef struct kheap_block_header kheap_block_header_t;

struct kheap_block_footer
{
    uint32 magic;
    kheap_block_header_t *header;
} __attribute__((packed));
typedef struct kheap_block_footer kheap_block_footer_t;


typedef struct kernel_heap
{
    ordered_array_t index;
    uint32 startAddress;
    uint32 endAddress;
    uint32 maxSize;
    uint32 size;
} kernel_heap_t;


void kheap_Init(void);
void* kmalloc(uint32 size, bool pageAligned);
void* kfree(void* address);

#endif