/******************************************************************************
* @file    Bitmap.h

* @brief   位图相关的头文件.
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
 * Date          : 2025年05月11日;
 * Revision         : 0.0.1;
 * Author           : ywBai;
 * Contents         : 初始创建文件
 ******************************************************************************/

#ifndef BITMAP_H
#define BITMAP_H

#include "Std_Types.h"
#include "Kheap.h"

#define INDEX_FROM_BIT(a) (a / 32)
#define OFFSET_FROM_BIT(a) (a % 32)

/**
 * @struct bitmap
 * @brief 定义位图数据结构。
 * 
 * 该结构体用于表示和管理位图，位图是一种高效存储布尔值的数据结构，
 * 每个比特位可代表一个布尔状态（如存在/不存在、使用/未使用等）。
 */
typedef struct bitmap
{
    uint32* array;  /**< 指向存储位图数据的 32 位无符号整数数组的指针。每个 uint32 元素可存储 32 个比特位。 */
    uint8 allocArray;  /**< 标志位，用于指示 array 所指向的内存是否为动态分配。非零值表示动态分配，零值表示非动态分配。 */
    int arraySize;  /**< 存储位图数据的数组的元素数量，即 array 数组中 uint32 元素的个数。 */
    int bits;  /**< 位图中总的比特位数，即该位图可表示的布尔状态数量。 */
} bitmap_t;

bitmap_t bitmap_Create(uint32* array, int numBits);
void bitmap_Init(bitmap_t* bitmap, uint32* array, uint32 numBits);
void bitmap_Set_Bit(bitmap_t* bitmap, uint32 bit);
void bitmap_Clear_Bit(bitmap_t* bitmap, uint32 bit);
bool bitmap_Get_Bit(bitmap_t* bitmap, uint32 bit);
bool bitmap_Find_First_Free_Bit(bitmap_t* bitmap, uint32* bit);
void bitmap_Clear(bitmap_t* bitmap);
bool bitmap_Allocate_First_Free_Bit(bitmap_t* bitmap, uint32* bit);
bool bitmap_Expand(bitmap_t* bitmap, uint32 expandSize);
void bitmap_Destroy(bitmap_t* bitmap);

#endif // !BITMAP_H