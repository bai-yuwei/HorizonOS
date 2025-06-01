/******************************************************************************
* @file    Page_Table.c
* @brief   页表相关的文件.
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

#include "Page_Table.h"

page_directory_t *currentPageDirectory = 0;

static bitmap_t phyFrameMap;
static uint32 bitArray[PHYSICAL_MEM_SIZE / PAGE_SIZE / 32];
static page_directory_t kernelPageDirectory;

static bool copyOnWriteReady = false;

static void free_Physical_Frame(uint32 frameAddress)
{
    bitmap_Clear_Bit(&phyFrameMap, frameAddress);
}

/**
 * @brief 释放指定虚拟地址对应的页表项。
 * 
 * 该函数用于释放指定虚拟地址对应的页表项（PTE）。如果该页表项存在且 freeFrame 标志为真，
 * 则会释放对应的物理帧。最后将页表项置零，标记为无效。
 * 
 * @param virtualAddress 要释放的页的虚拟地址。
 * @param freeFrame 布尔标志，指示是否释放对应的物理帧。
 */
/**
 * @brief 释放指定虚拟地址对应的页表项。
 * 
 * 该函数用于释放指定虚拟地址对应的页表项（PTE）。如果该页表项存在且 freeFrame 标志为真，
 * 则会释放对应的物理帧。最后将页表项置零，标记为无效，并重新加载页目录使更改生效。
 * 
 * @param virtualAddress 要释放的页的虚拟地址。
 * @param freeFrame 布尔标志，指示是否释放对应的物理帧。
 */
static void release_Page(uint32 virtualAddress, bool freeFrame)
{
    // 通过右移 12 位计算虚拟地址对应的页表项（PTE）索引
    // 在 32 位 x86 分页机制中，虚拟地址的中间 10 位用于索引页表项
    uint32 pteIndex = virtualAddress >> 12;
    // 计算页表项的虚拟地址，将页目录的虚拟基地址加上 PTE 索引，转换为 pte_t 类型指针
    pte_t* pte = (pte_t*)(PAGE_DIR_VIRTUAL + pteIndex);
    // 检查该页表项是否存在于内存中
    if (!pte->present)
    {
        // 若页表项不存在，则直接返回
        return;
    }
    // 获取该页表项对应的物理帧地址
    uint32 frame = pte->frame;
    // 检查是否需要释放对应的物理帧
    if (freeFrame)
    {
        // 调用 free_Physical_Frame 函数释放对应的物理帧
        free_Physical_Frame(frame);
    }
    // 将页表项对应的内存位置零，将该页表项标记为无效
    *((uint32*)pte) = 0;
    // 重新加载页目录，使页表项的更改生效
    reload_Page_Directory(currentPageDirectory);
}


/**
 * @brief 释放从指定虚拟地址开始的连续多个页表项。
 * 
 * 该函数用于释放从指定虚拟地址开始的连续多个页表项（PTE）。会遍历涉及的页目录项（PDE），
 * 对每个存在的页目录项，再遍历其中相关的页表项，并调用 release_Page 函数释放这些页表项。
 * 
 * @param virtualAddress 起始虚拟地址，释放操作将从该地址所在页开始。
 * @param pages 要释放的连续页的数量。
 * @param freeFrame 布尔标志，指示是否释放对应的物理帧。
 */
static void release_Pages(uint32 virtualAddress, uint32 pages, bool freeFrame)
{
    // 将虚拟地址按页边界（PAGE_SIZE）对齐，确保操作从页的起始位置开始
    virtualAddress = (virtualAddress / PAGE_SIZE) * PAGE_SIZE;
    // 计算起始虚拟地址对应的页表项（PTE）索引
    uint32 pteIndexStart = virtualAddress >> 12;
    // 计算结束时的页表项索引，即起始索引加上要释放的页数
    uint32 pteIndexEnd = pteIndexStart + pages;
    // 计算起始页表项对应的页目录项（PDE）索引
    uint32 pdeIndexStart = pteIndexStart >> 10;
    // 计算结束时的页目录项索引
    uint32 pdeIndexEnd = ((pteIndexEnd - 1) >> 10) + 1;

    // 遍历涉及的页目录项
    for (uint32 pdeIndex = pdeIndexStart; pdeIndex < pdeIndexEnd; pdeIndex++)
    {
        // 计算当前页目录项的虚拟地址，并转换为 pde_t 类型指针
        pde_t *pde = (pde_t*)(PAGE_DIR_VIRTUAL + pdeIndex);
        // 检查当前页目录项是否存在于内存中
        if (!pde->present)
        {
            // 若不存在，则跳过当前页目录项，继续下一个
            continue;
        }
        // 遍历当前页目录项对应的页表项
        for (uint32 pteIndex = max(pteIndexStart, pdeIndex * 1024); pteIndex < min(pteIndexEnd, (pdeIndex + 1) * 1024); pteIndex++)
        {
            // 调用 release_Page 函数释放当前页表项对应的页
            release_Page(pteIndex * PAGE_SIZE, freeFrame);
        }
    }
}

static int32 allocate_Physical_Frame(void)
{
    uint32 frameAddress = 0;
    if (!bitmap_Allocate_First_Free_Bit(&phyFrameMap, &frameAddress))
    {
        return -1;
    }
    return (int32)frameAddress;
}

/**
 * @brief 清空指定物理页的内容。
 * 
 * 该函数将指定物理地址所在页的所有内容清零。首先会将传入的地址按页边界对齐，
 * 然后遍历该页的每个 32 位字，将其值设置为 0。
 * 
 * @param addr 要清空内容的物理页的起始地址。
 */
static void clear_Page(uint32 addr)
{
    // 将传入的地址按页边界（4KB，即 0x1000）对齐，确保操作的是整个页
    addr = addr & 0xFFFFF000;
    // 循环遍历该页的每个 32 位字，PAGE_SIZE 为页的大小，除以 4 得到 32 位字的数量
    for (int i = 0; i < PAGE_SIZE / 4; i++)
    {
        // 将当前 32 位字的内存地址转换为 uint32 指针，并将其值设置为 0
        *(uint32 *)(addr + i * 4) = 0;
    }
}

// static int32 change_Cow_Frame_Refcount(uint32 frameAddress, int32 refCount)
// {
//     if (!)
// }

/**
 * @brief 将虚拟地址映射到指定物理帧。
 * 
 * 该函数用于将指定的虚拟地址映射到一个物理帧。首先根据虚拟地址计算页目录项（PDE）索引，
 * 检查对应的 PDE 是否存在。若不存在，则分配新的物理帧存储页表，并设置 PDE。
 * 接着计算页表项（PTE）索引，根据传入的物理帧参数设置 PTE。
 * 
 * @param virtualAddress 要映射的虚拟地址。
 * @param frame 要映射到的物理帧地址，若为负数则尝试分配新的物理帧。
 */
static void map_Page(uint32 virtualAddress, int32 frame)
{
    // 通过右移 22 位计算虚拟地址对应的页目录项（PDE）索引
    // 在 32 位 x86 分页机制中，虚拟地址的高 10 位用于索引页目录项
    uint32 pdeIndex = virtualAddress >> 22;
    // 将页目录的虚拟基地址转换为 pde_t 类型的指针，再加上 PDE 索引，得到对应的 PDE 指针
    pde_t *kernelPageDirVirtual = (pde_t*)PAGE_DIR_VIRTUAL;
    pde_t *pde = kernelPageDirVirtual + pdeIndex;

    // 检查该 PDE 对应的页目录表是否存在于内存中
    if (!pde->present)
    {
        // 若页目录表不存在，则分配一个新的物理帧用于存储页目录表
        int32 frameAddress = allocate_Physical_Frame();
        // 检查物理帧分配是否失败
        if (frameAddress < 0)
        {
            // 打印错误信息，提示无法为指定索引的页目录表分配物理帧
            monitor_Printf("couldn't alloc frame for page table on %d\n", pdeIndex);
            return;
        }
        // 将分配的物理帧地址赋值给 PDE 的 frame 字段
        pde->frame = frameAddress;
        // 将 PDE 的 present 位置为 1，表示对应的页目录表存在于内存中
        pde->present = 1;
        // 将 PDE 的读写位置为 1，表示页表可读写
        pde->rw = 1;
        // 将 PDE 的未使用位清零
        pde->unused = 0;
        // 重新加载页目录，使新的页目录表设置生效
        reload_Page_Directory(currentPageDirectory);
        // 清空新分配的页表所在的物理页，确保页表初始化为 0
        clear_Page(PAGE_TABLES_VIRTUAL + pdeIndex * PAGE_SIZE);
    }
    // 通过右移 12 位并与 0x3FF 进行按位与运算，计算虚拟地址对应的页表项（PTE）索引
    // 在 32 位 x86 分页机制中，虚拟地址的中间 10 位用于索引页表项
    uint32 pteIndex = virtualAddress >> 12;
    // 计算页表项的虚拟地址，将页表的虚拟基地址加上 PDE 索引对应的偏移，转换为 pte_t 类型指针
    pte_t* kernelPageTablesVirtual = (pte_t*)PAGE_TABLES_VIRTUAL;
    pte_t* pte = kernelPageTablesVirtual + pteIndex;
    // 若传入的物理帧地址大于 0，则直接使用该物理帧进行映射
    if (frame > 0)
    {
        // 将传入的物理帧地址赋值给 PTE 的 frame 字段
        pte->frame = frame;
        // 将 PTE 的 present 位置为 1，表示对应的页存在于内存中
        pte->present = 1;
        // 将 PTE 的读写位置为 1，表示页可读写
        pte->rw = 1;
        // 将 PTE 的用户位设置为 1，表示用户模式可以访问该页
        pte->user = 1;
        // 重新加载页目录，使新的页表项设置生效
        reload_Page_Directory(currentPageDirectory);
    }
    else
    {
        // 若 PTE 对应的页不存在于内存中
        monitor_Printf("pte->present = %x\n", pte->present);
        if (!pte->present)
        {
            // 尝试分配一个新的物理帧
            frame = allocate_Physical_Frame();
            // 检查物理帧分配是否失败
            if (frame < 0)
            {
                // 打印错误信息，提示无法为指定虚拟地址分配物理帧
                monitor_Printf("couldn't alloc frame for addr %x\n", virtualAddress);
            }
            // 将分配的物理帧地址赋值给 PTE 的 frame 字段
            pte->frame = frame;
            // 将 PTE 的 present 位置为 1，表示对应的页存在于内存中
            pte->present = 1;
            // 将 PTE 的读写位置为 1，表示页可读写
            pte->rw = 1;
            // 将 PTE 的用户位设置为 1，表示用户模式可以访问该页
            pte->user = 1;
            // 重新加载页目录，使新的页表项设置生效
            reload_Page_Directory(currentPageDirectory);
            // 清空新分配的物理页，确保页内容初始化为 0
            clear_Page(virtualAddress);
        }
    }
}

static void page_Fault_Handler(isr_params_t params)
{
    uint32 faultAddr;
    monitor_Printf("Page fault at %x\n", params.errCode);
    asm volatile("mov %%cr2, %0" : "=r"(faultAddr));
    int present = params.errCode & 0x1;
    int rw = params.errCode & 0x2;
    int userMode = params.errCode & 0x4;
    int reserved = params.errCode & 0x8;
    int id = params.errCode & 0x10;
    map_Page(faultAddr / PAGE_SIZE * PAGE_SIZE, -1);
    reload_Page_Directory(currentPageDirectory);
}

/**
 * @brief 启用 x86 架构的分页机制。
 * 
 * 该函数通过修改控制寄存器 CR0 的最高位（第 31 位）来启用分页功能。
 * 在 x86 架构中，CR0 寄存器的最高位为 1 时表示启用分页，为 0 时表示禁用分页。
 * 函数首先读取 CR0 寄存器的值，然后将其最高位置为 1，最后将修改后的值写回 CR0 寄存器。
 */
void enable_Paging(void)
{
    // 定义一个无符号 32 位整数，用于存储控制寄存器 CR0 的值
    uint32 cr0;
    // 使用内联汇编指令将控制寄存器 CR0 的值读取到变量 cr0 中
    // "=r"(cr0) 表示将结果存储到一个通用寄存器，然后赋值给 cr0
    // %%cr0 表示控制寄存器 CR0，双百分号用于转义百分号
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    // 将 cr0 的最高位（第 31 位）置为 1，0x80000000 的二进制表示为 1000 0000 ... 0000
    cr0 |= 0x80000000;
    // 使用内联汇编指令将修改后的 cr0 值写回到控制寄存器 CR0 中
    // "r"(cr0) 表示将 cr0 的值存储到一个通用寄存器，然后作为操作数传递给汇编指令
    asm volatile("mov %0, %%cr0":: "r"(cr0));
}

void reload_Page_Directory(page_directory_t *pageDirectory)
{
    asm volatile("mov %0, %%cr3" : : "r"(pageDirectory->pdePhyAddress));
}


/**
 * @brief 初始化分页机制。
 * 
 * 该函数用于初始化分页机制，包括初始化物理帧位图、设置内核页目录、
 * 释放内核二进制加载区域的页表项以及注册页错误中断处理函数。
 */
void page_Table_Init(void)
{
    // 为什么是 3MB，1MB boot，1MB kernel，1MB Kheap
    // 计算前 3MB 物理内存对应的位图数组元素数量，将这些元素初始化为全 1
    // 表示前 3MB 物理内存已被占用
    for (int i = 0; i < 3 * 1024 * 1024 / PAGE_SIZE / 32; i++)
    {
        bitArray[i] = 0xffffffff;
    }
    // 创建物理帧位图，使用 bitArray 数组管理物理内存帧
    // PHYSICAL_MEM_SIZE / PAGE_SIZE 表示物理内存总帧数
    phyFrameMap = bitmap_Create(bitArray, PHYSICAL_MEM_SIZE / PAGE_SIZE);
    // 将最后一个物理帧标记为已使用
    bitmap_Set_Bit(&phyFrameMap, PHYSICAL_MEM_SIZE / PAGE_SIZE - 1);
    // 设置内核页目录的物理地址
    kernelPageDirectory.pdePhyAddress = KERNEL_PAGE_DIR_PHY;
    // 将当前页目录指针指向内核页目录
    currentPageDirectory = &kernelPageDirectory;
    // 释放从 0xFFFFFFFF - KERNEL_BIN_LOAD_SIZE + 1 开始的连续页表项
    // KERNEL_BIN_LOAD_SIZE / PAGE_SIZE 表示要释放的页数
    // true 表示同时释放对应的物理帧
    release_Pages(0xFFFFFFFF - KERNEL_BIN_LOAD_SIZE + 1, KERNEL_BIN_LOAD_SIZE / PAGE_SIZE, true);
    // 注册页错误中断处理函数，页错误中断号为 14
    register_Interrupt_Handler(14, page_Fault_Handler);
}

void test_Page_Table(void)
{
    monitor_Printf("test page table\n");
    uint32 *addr = (uint32*)0xC;
    *addr = 1;
}


void page_Table_Test(void)
{
    uint32 i = 0x10000000;
    monitor_Printf("page table test\n");
    while (1)
    { 
        monitor_Printf("addr = %x\n", i);
        uint32 *addr = (uint32*)i;
        *addr = 1;
        i++;
    }
    
}