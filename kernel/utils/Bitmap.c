/******************************************************************************
* @file    Bitmap.c
* @brief   位图相关的文件.
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

#include "Bitmap.h"

bitmap_t bitmap_Create(uint32* array, int numBits)
{
    bitmap_t bitmap;
    bitmap_Init(&bitmap, array, numBits);
    return bitmap;
}

// 位图初始化函数
void bitmap_Init(bitmap_t* bitmap, uint32* array, uint32 numBits)
{
    bitmap->bits = numBits;
    bitmap->arraySize = numBits / 32;
    if (array == nullptr)
    {
        array = (uint32*)kmalloc(bitmap->arraySize * sizeof(uint32), NOT_PAGE_ALIGNED);
        bitmap->allocArray = true;
    }
    else
    {
        bitmap->allocArray = false;
    }
    for (int32 i = 0; i < bitmap->arraySize; i++)
    {
        array[i] = 0;
    }
    bitmap->array = array;
}

void bitmap_Set_Bit(bitmap_t* bitmap, uint32 bit)
{
    uint32 arrayIndex = INDEX_FROM_BIT(bit);
    uint32 offset = OFFSET_FROM_BIT(bit);
    bitmap->array[arrayIndex] |= (1 << offset);
}

void bitmap_Clear_Bit(bitmap_t* bitmap, uint32 bit)
{
    uint32 arrayIndex = INDEX_FROM_BIT(bit);
    uint32 offset = OFFSET_FROM_BIT(bit);
    bitmap->array[arrayIndex] &= ~(1 << offset);
}

bool bitmap_Get_Bit(bitmap_t* bitmap, uint32 bit)
{
    uint32 arrayIndex = INDEX_FROM_BIT(bit);
    uint32 offset = OFFSET_FROM_BIT(bit);
    return (bitmap->array[arrayIndex] & (1 << offset)) != 0;
}

/**
 * @brief 查找位图中第一个空闲位（值为 0 的位）。
 *
 * 该函数会遍历位图数组，查找第一个值不为全 1 的元素，
 * 然后在该元素的 32 位中查找第一个值为 0 的位。
 * 如果找到，将该位的索引存储在传入的指针所指向的位置，并返回 true；
 * 如果未找到，返回 false。
 *
 * @param bitmap 指向位图结构体的指针，包含位图的相关信息。
 * @param bit 指向一个无符号 32 位整数的指针，用于存储找到的第一个空闲位的索引。
 * @return bool 若找到空闲位返回 true，否则返回 false。
 */
bool bitmap_Find_First_Free_Bit(bitmap_t* bitmap, uint32* bit)
{
    // 遍历位图数组中的每个元素
    for (uint32 i = 0; i < bitmap->arraySize; i++)
    {
        // 检查当前元素是否不为全 1（即存在空闲位）
        if (bitmap->array[i] != 0xFFFFFFFF)
        {
            // 遍历当前元素的 32 位
            for (uint32 j = 0; j < 32; j++)
            {
                // 检查当前位是否为 0（空闲位）
                if (!bitmap_Get_Bit(bitmap, i * 32 + j))
                {
                    // 将找到的空闲位的索引存储到传入的指针所指向的位置
                    *bit = i * 32 + j;
                    // 找到空闲位，返回 true
                    return true;
                }
            }
        }
    }
    // 未找到空闲位，返回 false
    return false;
}

void bitmap_Clear(bitmap_t* bitmap)
{
    for (uint32 i = 0; i < bitmap->arraySize; i++)
    {
        bitmap->array[i] = 0;
    }
}

bool bitmap_Allocate_First_Free_Bit(bitmap_t* bitmap, uint32* bit)
{
    if (bitmap_Find_First_Free_Bit(bitmap, bit))
    {
        bitmap_Set_Bit(bitmap, *bit);
        return true;
    }
    return false;
}

/**
 * @brief 扩展位图的大小。
 *
 * 该函数用于扩展位图的大小，会分配新的内存空间来存储扩展后的位图数据，
 * 并将原有的位图数据复制到新的内存空间中。如果原有的位图数据是通过动态分配得到的，
 * 则会释放原有的内存空间。
 *
 * @param bitmap 指向位图结构体的指针，包含位图的相关信息。
 * @param expandSize 要扩展到位图的新总位数。
 * @return bool 若扩展成功返回 true，若内存分配失败则返回 false。
 */
bool bitmap_Expand(bitmap_t* bitmap, uint32 expandSize)
{
    // 计算扩展后的新总位数
    uint32 newSize = expandSize;
    // 计算扩展后所需的数组元素数量，每个元素可存储 32 位
    uint32 newArraySize = newSize / 32;
    // 动态分配新的内存空间，用于存储扩展后的位图数据
    uint32* newArray = (uint32*)kmalloc(newArraySize * sizeof(uint32), NOT_PAGE_ALIGNED);
    // 检查内存分配是否失败
    if (newArray == nullptr)
    {
        // 内存分配失败，返回 false
        return false;
    }
    // 将新分配的内存空间初始化为 0
    for (uint32 i = 0; i < newArraySize; i++)
    {
        newArray[i] = 0;
    }
    // 将原有的位图数据复制到新分配的内存空间中
    for (uint32 i = 0; i < bitmap->arraySize; i++)
    {
        newArray[i] = bitmap->array[i];
    }
    // 检查原有的位图数据是否是通过动态分配得到的
    if (bitmap->allocArray)
    {
        // 若是动态分配的，释放原有的内存空间
        kfree(bitmap->array);
    }
    // 更新位图结构体中的数组指针，指向新分配的内存空间
    bitmap->array = newArray;
    // 更新位图结构体中的数组元素数量
    bitmap->arraySize = newArraySize;
    // 更新位图结构体中的总位数
    bitmap->bits = newSize;
    // 标记位图数据是通过动态分配得到的
    bitmap->allocArray = true;
    // 扩展成功，返回 true
    return true;
}

void bitmap_Destroy(bitmap_t* bitmap)
{
    if (bitmap->allocArray)
    {
        kfree(bitmap->array);
    }
}