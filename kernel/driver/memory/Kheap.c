/******************************************************************************
* @file    Kheap.c
* @brief   内核堆相关的文件.
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
* Date          : 2025年05月17日;
* Revision         : 0.0.1;
* Author           : ywBai;
* Contents         :
******************************************************************************/
#include "Kheap.h"
#include "Yieldlock.h"

static yieldlock_t kheapLock;
static kernel_heap_t kheap;

static int32 kheap_Block_Compare(void *a, void *b)
{
    uint32 size1 = ((kheap_block_header_t *)a)->size;
    uint32 size2 = ((kheap_block_header_t *)b)->size;
    return size1 - size2;
}

/**
 * @brief 创建一个新的堆块。
 *
 * 此函数用于在指定的起始地址创建一个新的堆块，设置块的头部和尾部信息。
 * 堆块包含一个头部和一个尾部，头部存储块的大小和是否空闲的信息，
 * 尾部存储魔术数字和指向头部的指针，用于验证块的完整性。
 *
 * @param startAddress 堆块的起始地址。
 * @param size 堆块的数据部分大小，不包含头部和尾部的元数据大小。
 * @param isFree 标记堆块是否空闲，1 表示空闲，0 表示已使用。
 * @return kheap_block_header_t* 指向新创建堆块头部的指针。
 */
static kheap_block_header_t* make_Block(uint32 startAddress, uint32 size, bool isFree)
{
    // 计算堆块的结束地址，包含头部、数据部分和尾部的大小
    uint32 endAddress = startAddress + size + BLOCK_META_SIZE;
    // 将起始地址转换为堆块头部指针，并赋值给 header
    kheap_block_header_t *header = (kheap_block_header_t *)startAddress;
    header->magic = KHEAP_MAGIC;
    // 设置堆块头部的大小信息
    header->size = size;
    // 设置堆块头部的空闲标记
    header->isFree = isFree;
    // 计算堆块尾部的地址，并将其转换为堆块尾部指针
    kheap_block_footer_t *footer = (kheap_block_footer_t *)(endAddress - FOOTER_SIZE);
    // 设置堆块尾部的魔术数字，用于验证堆块的完整性
    footer->magic = KHEAP_MAGIC;
    // 设置堆块尾部指向头部的指针
    footer->header = header;
    // 返回指向新创建堆块头部的指针
    return header;
}

/**
 * @brief 将给定的地址按页大小对齐。
 *
 * 此函数用于将输入的地址值按页大小进行对齐。在内存管理中，
 * 经常需要将地址对齐到页边界，以提高内存访问效率。
 * 页大小通常为 4KB（即 0x1000 字节），在本函数中通过 0xFFF 进行掩码判断。
 *
 * @param num 待对齐的地址值。
 * @return uint32 对齐后的地址值。
 */
static uint32 align_Page(uint32 num)
{
    // 检查地址是否已经按页对齐。0xFFF 是 4KB 页大小的掩码，
    // 如果 num 与 0xFFF 按位与的结果不为 0，说明地址未对齐。
    if ((num & 0xFFF) != 0)
    {
        // 先清除地址的低 12 位（即 0xFFF 对应的位），
        // 然后加上页大小（PAGE_SIZE），得到对齐后的地址。
        return (num & ~(0xFFF)) + PAGE_SIZE;
    }
    // 如果地址已经按页对齐，直接返回原始地址。
    return num;
}

/**
 * @brief 查找满足条件的空闲堆块。
 *
 * 该函数会遍历内核堆实例中的所有堆块，查找满足指定大小需求的空闲堆块。
 * 根据是否需要页对齐，采用不同的查找逻辑。若找到合适的堆块，
 * 则返回该堆块在索引数组中的位置，并通过 allocPosition 指针返回分配位置；
 * 若未找到，返回 -1。
 *
 * @param heap 指向内核堆实例的指针。
 * @param size 所需分配的内存大小。
 * @param pageAligned 是否需要页对齐，true 表示需要，false 表示不需要。
 * @param allocPosition 用于存储分配位置的指针。
 * @return int32 找到的空闲堆块在索引数组中的位置，若未找到则返回 -1。
 */
static int32 find_Free_Block(kernel_heap_t *heap, uint32 size, bool pageAligned, uint32* allocPosition)
{
    // 遍历内核堆实例的索引数组
    for (int i = 0; i < heap->index.size; i++)
    {
        // 从索引数组中获取当前堆块的头部指针
        kheap_block_header_t *header = (kheap_block_header_t *)ordered_Array_Get(&heap->index, i);
        // 计算当前堆块数据部分的起始地址，即头部之后的位置
        uint32 startAddress = (uint32)header + HEADER_SIZE;

        // 如果需要页对齐
        if (pageAligned)
        {
            // 计算当前堆块数据部分的结束地址
            uint32 endAddress = startAddress + header->size;
            // 计算从当前堆块数据起始地址开始的下一个页对齐地址
            uint32 nextPageAligned = align_Page(startAddress);

            // 检查下一个页对齐地址加上所需内存大小是否在当前堆块范围内
            while (nextPageAligned + size <= endAddress)
            {
                // 检查页对齐地址与堆块数据起始地址之间的空闲空间是否足够分割出一个新的元数据块
                if (nextPageAligned - startAddress > BLOCK_META_SIZE)
                {
                    // 若满足条件，将页对齐地址作为分配位置
                    *allocPosition = nextPageAligned;
                    // 返回当前堆块在索引数组中的位置
                    return i;
                }
                // 移动到下一个页对齐地址
                nextPageAligned += PAGE_SIZE;
            }
        }
        // 如果不需要页对齐，且当前堆块的大小满足所需内存大小
        else if (header->size >= size)
        {
            // 将当前堆块数据起始地址作为分配位置
            *allocPosition = startAddress;
            // 返回当前堆块在索引数组中的位置
            return i;
        }
    }
    // 未找到满足条件的空闲堆块，返回 -1
    return -1;
}

static uint32 kheap_Expand(kernel_heap_t *heap, uint32 size)
{
    uint32 expandSize = align_Page(size);
    if (expandSize <= 0)
    {
        return 0;
    }
    uint32 newEndAddress = heap->endAddress + expandSize;
    if (newEndAddress > heap->maxSize)
    {
        return 0;
    }
    heap->size += expandSize;
    return expandSize;
}

/**
 * @brief 创建一个内核堆实例。
 *
 * 此函数用于初始化一个内核堆，设置堆的索引、起始地址、结束地址、当前大小和最大大小，
 * 并创建一个初始的堆块，将其插入到有序数组索引中。
 *
 * @param startAddress 内核堆的起始地址。
 * @param endAddress 内核堆的结束地址。
 * @param maxSize 内核堆允许的最大大小。
 * @return kernel_heap_t 初始化后的内核堆实例。
 */
static kernel_heap_t kernel_Heap_Create(uint32 startAddress, uint32 endAddress, uint32 maxSize)
{
    // 定义一个内核堆结构体变量
    kernel_heap_t heap;
    ordered_array_return_t rtn = 0;
    // 创建一个有序数组作为堆的索引，用于管理堆块
    // 起始地址为传入的 startAddress，数组元素数量为 KHEAP_INDEX_NUM，比较函数为 standard_Compare
    heap.index = ordered_Array_Create((void *)startAddress, KHEAP_INDEX_NUM, &kheap_Block_Compare);
    // 更新起始地址，跳过索引占用的内存空间
    startAddress += (sizeof(void*) * KHEAP_INDEX_NUM);
    // 设置堆的起始地址
    heap.startAddress = startAddress;
    // 设置堆的结束地址
    heap.endAddress = endAddress;
    // 计算并设置堆的当前大小
    heap.size = endAddress - startAddress;
    // 设置堆允许的最大大小
    heap.maxSize = maxSize;
    // 在堆的起始地址创建一个新的堆块，数据部分大小为堆当前大小减去元数据大小，标记为未使用
    make_Block(startAddress, endAddress - startAddress - BLOCK_META_SIZE, IS_FREE);
    // 将新创建的堆块头部地址插入到堆的有序数组索引中
    rtn = ordered_Array_Insert(&heap.index, (void *)startAddress);
    if (rtn != 0)
    {
        monitor_Print("ordered_Array_Insert failed\n");
    }
    // 返回初始化后的内核堆实例
    return heap;
}


/**
 * @brief 在内核堆中分配指定大小的内存。
 *
 * 该函数会尝试在内核堆中找到满足指定大小需求的空闲内存块。如果需要，会扩展堆的大小。
 * 根据是否需要页对齐，采用不同的分配逻辑。若找到合适的内存块，会将其标记为已使用，
 * 并可能对剩余的空闲空间进行分割处理。
 *
 * @param heap 指向内核堆实例的指针。
 * @param size 所需分配的内存大小。
 * @param pageAligned 是否需要页对齐，true 表示需要，false 表示不需要。
 * @return void* 指向分配的内存块的指针，若分配失败可能返回 NULL（递归调用失败时）。
 */
static void* alloc(kernel_heap_t *heap, uint32 size, bool pageAligned)
{
    // 用于存储分配的内存块的起始地址
    uint32 allocPosition = 0;
    // 调用 find_Free_Block 函数查找满足条件的空闲堆块，返回该堆块在索引数组中的位置
    int32 index = find_Free_Block(heap, size, pageAligned, &allocPosition);
    // 若未找到满足条件的空闲堆块
    if (index < 0)
    {
        // 记录堆原来的结束地址
        uint32 oldEndAddress = heap->endAddress;
        // 调用 kheap_Expand 函数扩展堆的大小，扩展大小包含所需内存和元数据大小
        uint32 expandSize = kheap_Expand(heap, size + BLOCK_META_SIZE);
        // 获取原堆块的尾部指针
        kheap_block_footer_t *oldFooter = (kheap_block_footer_t *)(oldEndAddress - FOOTER_SIZE);
        // 通过尾部指针获取原堆块的头部指针
        kheap_block_header_t *oldHeader = oldFooter->header;
        // 若原堆块是空闲的
        if (oldHeader->isFree)
        {
            // 合并原堆块和新扩展的空间，创建一个新的空闲堆块
            make_Block((uint32)oldHeader, oldHeader->size + expandSize, IS_FREE);
            // 从索引数组中移除原堆块的头部地址
            int32 remove = ordered_Array_Remove(&heap->index, (void *)oldHeader);
            // 将新的堆块头部地址插入到索引数组中
            ordered_Array_Insert(&heap->index, (void *)oldHeader);
        }
        else
        {
            // 在原堆块结束地址处创建一个新的空闲堆块，其大小为扩展大小减去元数据大小
            kheap_block_header_t *newHeader = make_Block(oldEndAddress, expandSize - BLOCK_META_SIZE, IS_FREE);
            // 将新创建的堆块头部地址插入到索引数组中
            ordered_Array_Insert(&heap->index, (void *)newHeader);
        }   
        // 递归调用 alloc 函数，再次尝试分配内存
        return alloc(heap, size, pageAligned);
    }
    // 从索引数组中获取找到的空闲堆块的头部指针
    kheap_block_header_t *header = (kheap_block_header_t *)ordered_Array_Get(&heap->index, index);
    // 获取该堆块的数据部分大小
    uint32 blockSize = header->size;
    // 从索引数组中移除该堆块的头部地址
    ordered_Array_Remove_Index(&heap->index, index);
    // 如果需要页对齐
    if (pageAligned)
    {
        // 计算分配内存块的头部地址
        kheap_block_header_t *allocHeader = (kheap_block_header_t*)(allocPosition - HEADER_SIZE);
        // 若分配内存块的头部地址大于原堆块的头部地址
        if (allocHeader > header)
        {
            // 计算需要分割出来的空闲块的大小
            uint32 cutBlockSize = (uint32)allocHeader - (uint32)header;
            // 创建一个新的空闲堆块，其大小为分割出来的大小减去元数据大小
            make_Block((uint32)header, cutBlockSize - BLOCK_META_SIZE, IS_FREE);
            // 将新创建的空闲堆块头部地址插入到索引数组中
            ordered_Array_Insert(&heap->index, (void *)header);
            // 更新当前处理的堆块头部指针为分配内存块的头部指针
            header = allocHeader;
            // 原堆块大小减去分割出去的大小
            blockSize -= cutBlockSize;
        }
    }
    // 计算分配所需内存后剩余的大小
    uint32 remainSize = blockSize - size;
    // 若剩余大小小于等于元数据大小，不进行分割，将整个堆块分配出去
    if (remainSize <= BLOCK_META_SIZE)
    {
        size = blockSize;
        remainSize = 0;
    }
    // 创建一个新的已使用堆块，其大小为所需分配的内存大小
    kheap_block_header_t *selectedHeader = make_Block((uint32)header, size, NOT_FREE);
    // 若剩余大小大于 0，分割出一个新的空闲堆块
    if (remainSize > 0)
    {
        // 在已分配堆块之后创建一个新的空闲堆块
        kheap_block_header_t *remainHeader = make_Block((uint32)header + BLOCK_META_SIZE + size, remainSize - BLOCK_META_SIZE, IS_FREE);
        // 将新创建的空闲堆块头部地址插入到索引数组中
        ordered_Array_Insert(&heap->index, (void *)remainHeader);
    }
    // 返回分配的内存块的起始地址
    return (void*)(allocPosition);
}

/**
 * @brief 释放内核堆中指定地址的内存块。
 *
 * 该函数会将指定地址的内存块标记为空闲状态，并尝试与相邻的空闲内存块合并，
 * 以减少内存碎片。最后将合并后的空闲内存块插入到内核堆的索引数组中。
 *
 * @param heap 指向内核堆实例的指针。
 * @param freedAddress 指向需要释放的内存块起始地址的指针。
 */
static void free(void* heap, void* freedAddress)
{
    // 若释放地址为空指针，直接返回，不进行任何操作
    if (freedAddress == nullptr)
    {
        return;
    }
    // 通过释放地址计算该内存块的头部地址，并转换为堆块头部指针
    kheap_block_header_t *header = (kheap_block_header_t *)((uint32)freedAddress - HEADER_SIZE);
    // 通过释放地址和内存块大小计算该内存块的尾部地址，并转换为堆块尾部指针
    kheap_block_footer_t *footer = (kheap_block_footer_t *)((uint32)freedAddress + header->size);
    // 将该内存块标记为空闲状态
    header->isFree = IS_FREE;
    // 初始化新的堆块头部指针，指向当前释放的内存块头部
    kheap_block_header_t *newHeader = header;
    // 计算下一个内存块的头部地址，并转换为堆块头部指针
    kheap_block_header_t *nextHeader = (kheap_block_header_t *)((uint32)footer + FOOTER_SIZE);
    // 检查下一个内存块的魔术数字是否正确，并且该内存块是否为空闲状态
    if (nextHeader->magic == KHEAP_MAGIC && nextHeader->isFree) 
    {
        // 若条件满足，合并当前内存块和下一个内存块，创建一个新的空闲堆块
        make_Block((uint32)header, header->size + BLOCK_META_SIZE + nextHeader->size, IS_FREE);
        // 从内核堆的索引数组中移除下一个内存块的头部地址
        int32 index = ordered_Array_Remove(&((kernel_heap_t *)heap)->index, (void *)nextHeader);
    }
    // 计算前一个内存块的尾部地址，并转换为堆块尾部指针
    kheap_block_footer_t *prevFooter = (kheap_block_footer_t *)((uint32)header - FOOTER_SIZE);
    // 检查前一个内存块的魔术数字是否正确，并且该内存块是否为空闲状态
    if (prevFooter->magic == KHEAP_MAGIC && prevFooter->header->isFree)
    {
        // 获取前一个内存块的头部指针
        kheap_block_header_t *prevHeader = prevFooter->header;
        // 合并前一个内存块和当前内存块，创建一个新的空闲堆块
        make_Block((uint32)prevHeader, prevHeader->size + BLOCK_META_SIZE + header->size, IS_FREE);
        // 从内核堆的索引数组中移除当前内存块的头部地址
        int32 index = ordered_Array_Remove(&((kernel_heap_t *)heap)->index, (void *)header);
        // 更新当前处理的堆块头部指针为前一个内存块的头部指针
        header = prevHeader;
        // 更新新的堆块头部指针为合并后的头部指针
        newHeader = header;
    }
    // 将合并后的空闲堆块头部地址插入到内核堆的索引数组中
    ordered_Array_Insert(&((kernel_heap_t *)heap)->index, (void *)newHeader);
}


void kheap_Init(void)
{
    yieldlock_Init(&kheapLock);
    kheap = kernel_Heap_Create(KHEAP_START, KHEAP_START + KHEAP_MIN_SIZE, KHEAP_MAX);
}

void* kmalloc(uint32 size, bool pageAligned)
{
    if (size == 0)
    {
        return nullptr;
    }
    return alloc(&kheap, size, pageAligned);
}

void kfree(void* address)
{
    if (address == nullptr)
    {
        return;
    }
    free(&kheap, address);
    return;
}

void kheap_Test(void)
{
    monitor_Printf("kheap_Test\n");
    uint32 *p1 = (uint32 *)kmalloc(0x1000, PAGE_ALIGNED);
    *p1 = 100;
    monitor_Printf("p1: %x\n", p1);
    monitor_Printf("*p1: %d\n", *p1);
    uint32 *p2 = (uint32 *)kmalloc(0x1000, PAGE_ALIGNED);
    *p2 = 101;
    monitor_Printf("p2: %x\n", p2);
    monitor_Printf("*p2: %d\n", *p2);
    kfree(p1);
    kfree(p2);
}
