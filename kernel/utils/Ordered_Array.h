/******************************************************************************
* @file    Ordered_Array.h

* @brief   内核有序数组管理相关的头文件.
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

#ifndef ORDERED_ARRAY_H
#define ORDERED_ARRAY_H

#include "Std_Types.h"
#include "Monitor.h"

typedef uint8 ordered_array_return_t;

#define OK 0
#define OUT_OF_RANGE 1

/**
 * @typedef comparator_t
 * @brief 定义一个函数指针类型，用于比较两个通用指针指向的数据。
 * 
 * 该函数指针类型指向的函数接收两个 `void*` 类型的参数，
 * 这意味着它可以处理任意类型的数据。函数返回一个 32 位有符号整数，
 * 用于表示两个参数所指向数据的大小关系。
 * 
 * @param a 指向第一个待比较数据的通用指针。
 * @param b 指向第二个待比较数据的通用指针。
 * @return 若第一个数据小于第二个数据，返回小于 0 的值；
 *         若两个数据相等，返回 0；
 *         若第一个数据大于第二个数据，返回大于 0 的值。
 */
typedef int32 (*comparator_t)(void*, void*);

/**
 * @struct Ordered_Array
 * @brief 定义一个有序数组结构体，用于管理按特定顺序排列的元素集合。
 * 
 * 该结构体封装了有序数组的关键信息，包括存储元素的内存地址、
 * 当前元素数量、最大容量以及用于元素排序的比较函数。
 */
typedef struct Ordered_Array
{
    void** array;  /**< 指向存储元素指针的数组的指针。由于使用 `void*`，可存储任意类型对象的指针。 */
    uint32 size;   /**< 有序数组中当前实际存储的元素数量。取值范围为 0 到 `maxSize`。 */
    uint32 maxSize;/**< 有序数组能够容纳的最大元素数量。当 `size` 达到此值时，数组已满。 */
    comparator_t comparator; /**< 指向比较函数的指针，该函数用于确定元素在数组中的排序顺序。 */
} ordered_array_t;

int32 standard_Compare(void *a, void *b);
ordered_array_t ordered_Array_Create(void* array, uint32 maxSize, comparator_t comparator);
ordered_array_return_t ordered_Array_Insert(ordered_array_t *orderedArray, void *element);
ordered_array_return_t ordered_Array_Remove(ordered_array_t *orderedArray, void *element);
uint32 ordered_Array_Search(ordered_array_t *orderedArray, void *element);
void* ordered_Array_Get(ordered_array_t *orderedArray, uint32 index);
ordered_array_return_t ordered_Array_Remove_Index(ordered_array_t *orderedArray, uint32 index);
void ordered_Array_Test();
#endif